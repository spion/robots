all: sonar robot drawani drawani3d boxer

robot: robot.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall robot.cpp -o robot

drawani: drawani.cpp common.hpp
	${CXX} ${CXXFLAGS} drawani.cpp -o drawani  -Wall -lglut -lGLU -lGL 

drawlines: drawlines.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall drawlines.cpp -o drawlines -lglut -lGLU -lGL -lc 


sonar: sonar.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall sonar.cpp -o sonar

drawani3d: drawani3d.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall drawani3d.cpp -o drawani3d -lglut -lGLU -lGL 

boxer: boxer.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall boxer.cpp -o boxer
