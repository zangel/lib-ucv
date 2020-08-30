

set(math_headers
  include/baldzarika/math/config.h
  include/baldzarika/math/constant.h
  include/baldzarika/math/fixed_point.h
  include/baldzarika/math/round.h
  include/baldzarika/math/box2.h
  include/baldzarika/math/point2.h
  include/baldzarika/math/size2.h
  include/baldzarika/math/jacobi.h
  include/baldzarika/math/svd.h
  include/baldzarika/math/solve.h
  include/baldzarika/math/vector.h
  include/baldzarika/math/vector_ops.h
  include/baldzarika/math/matrix.h
  include/baldzarika/math/matrix_ops.h
  include/baldzarika/math/utils.h
)

set(math_sources
	src/baldzarika/math/config.cpp
)

add_library(math STATIC
	${math_headers}
	${math_sources}
)
