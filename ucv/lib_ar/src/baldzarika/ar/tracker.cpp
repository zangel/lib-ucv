#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/marker.h>

namespace baldzarika { namespace ar {

	boost::uint32_t const tracker::DEFAULT_SURF_OCTAVES=3;
	boost::uint32_t const tracker::DEFAULT_SURF_INTERVALS=4;
	boost::uint32_t const tracker::DEFAULT_SURF_SAMPLE_STEPS=2;
	float const tracker::DEFAULT_SURF_TRESHOLD=4.0e-4f;

	tracker::marker_state::marker_state(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker> const &m)
		: m_tracker(t)
		, m_marker(m)
		, m_detected(false)
	{
	}

	tracker::marker_state::~marker_state()
	{
	}

	bool tracker::marker_state::is_detected() const
	{
		return m_detected;
	}

	boost::shared_ptr<marker> const& tracker::marker_state::get_marker() const
	{
		return m_marker;
	}

	void tracker::marker_state::collect_features()
	{
		if(boost::shared_ptr<tracker> t=m_tracker.lock())
		{
			if(m_marker && !m_marker->get_size().empty())
			{
				ucv::surf surf_(
					m_marker->get_size(),
					t->m_surf.octaves(),
					t->m_surf.intervals(),
					t->m_surf.sample_step(),
					t->m_surf.treshold()
					);
				if(surf_.update(m_marker->get_view()))
				{
					surf_.build_response_layers();
					surf_.detect(m_features);
					surf_.describe(m_features);
				}
			}
		}
	}

	void tracker::marker_state::set_detected(bool d)
	{
		m_detected=d;
		if(boost::shared_ptr<tracker> t=m_tracker.lock())
			t->m_marker_state_changed(shared_from_this(), SC_DETECTION);
	}

	tracker::tracker(ucv::size2ui const &fs)
		: m_ios()
		, m_ios_work(m_ios)
		, m_worker()
		, m_surf(fs, DEFAULT_SURF_OCTAVES, DEFAULT_SURF_INTERVALS, DEFAULT_SURF_SAMPLE_STEPS, DEFAULT_SURF_TRESHOLD)
	{

	}

	tracker::~tracker()
	{
		stop();
	}

	ucv::size2ui tracker::get_frame_size() const
	{
		return m_surf.size();
	}

	bool tracker::set_frame_size(ucv::size2ui const &fs)
	{
		if(is_started())
			return false;
		m_surf.resize(fs);
		return true;
	}

	boost::shared_ptr<tracker::marker_state> tracker::add_marker(boost::shared_ptr<marker> const &m)
	{
		if(is_started())
			return boost::shared_ptr<marker_state>();
		boost::shared_ptr<marker_state> new_marker_state( new marker_state(shared_from_this(), m) );
		if(m_marker_states.push_back(new_marker_state).second)
			return new_marker_state;
		return boost::shared_ptr<marker_state>();
	}

	bool tracker::remove_marker(boost::shared_ptr<marker> const &m)
	{
		return false;
	}

	bool tracker::remove_marker_state(boost::shared_ptr<marker_state> const &ms)
	{
		return false;
	}

	tracker::marker_state::changed_signal_t tracker::marker_state_changed() const
	{
		return m_marker_state_changed;
	}


	bool tracker::start()
	{
		if(is_started())
			return true;
		m_worker=boost::thread(boost::bind(&boost::asio::io_service::run, &m_ios)).move();
		if(is_started())
		{
			m_ios.post( boost::bind(&tracker::on_start, this) );
			return true;
		}
		return false;
	}

	bool tracker::is_started() const
	{
		return m_worker.get_id()!=boost::thread::id();
	}

	bool tracker::stop()
	{
		if(!is_started())
			return false;
		m_ios.post( boost::bind(&tracker::on_stop, this) );
		m_ios.stop();
		m_worker.join();
		if(!is_started() && m_ios.stopped())
		{
			m_ios.reset();
			return true;
		}
		return false;
	}

	bool tracker::update(ucv::surf::gray_view_t gv)
	{
		if(!is_started())
			return false;
		{
			boost::mutex::scoped_try_lock try_update(m_surf_sync);
			if(try_update)
			{
				if(m_surf.update(gv))
					m_ios.post( boost::bind(&tracker::on_update, this) );
			}
		}
		return true;
	}

	void tracker::on_start()
	{
		marker_states_t::index<marker_state::detected_tag>::type &markers_by_detected=m_marker_states.get<marker_state::detected_tag>();
		for(marker_states_t::iterator it_marker=m_marker_states.begin();it_marker!=m_marker_states.end();++it_marker)
		{
			boost::shared_ptr<marker_state> ms=*it_marker;
			BOOST_ASSERT(ms);
			markers_by_detected.modify(
				m_marker_states.project<marker_state::detected_tag>(it_marker),
				boost::bind(
					&marker_state::set_detected,
					_1,
					false
				)
			);
			ms->collect_features();
		}
	}

	void tracker::on_update()
	{
		boost::mutex::scoped_lock lock_surf(m_surf_sync);

		std::vector<feature_point_t> surf_features;
		m_surf.build_response_layers();
		m_surf.detect(surf_features);
		m_surf.describe(surf_features);
	}

	void tracker::on_stop()
	{
	}


} //namespace ar
} //namespace baldzarika
