all: sonar robot drawani drawani3d boxer

robot: robot.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall robot.cpp -o robot

drawani: drawani.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall -lglut -lGL drawani.cpp -o drawani

drawlines: drawlines.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall -lglut -lGL drawlines.cpp -o drawlines


sonar: sonar.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall sonar.cpp -o sonar

drawani3d: drawani3d.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall -lglut -lGL drawani3d.cpp -o drawani3d

boxer: boxer.cpp common.hpp
	${CXX} ${CXXFLAGS} -Wall boxer.cpp -o boxer
