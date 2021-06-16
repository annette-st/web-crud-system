include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        cpr-ext
		PREFIX ${CMAKE_BINARY_DIR}/thirdparty/cpr
        GIT_REPOSITORY https://github.com/whoshuu/cpr.git
		GIT_PROGRESS TRUE
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
		CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
			-DCPR_ENABLE_SSL=OFF
			-DCPR_BUILD_TESTS=OFF
			-DBUILD_SHARED_LIBS=OFF
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(cpr-ext install_dir)

macro(DEF_CPR_LIB NAME LIB)
	add_library(${NAME} STATIC IMPORTED GLOBAL)
	add_library(thirdparty::${NAME} ALIAS ${NAME})
	set_property(TARGET ${NAME} PROPERTY IMPORTED_LOCATION ${install_dir}/lib/${LIB})
	set_property(TARGET ${NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir})
	add_dependencies(${NAME} cpr-ext)
endmacro()

DEF_CPR_LIB(cpr libcpr.a)
DEF_CPR_LIB(cpr-curl libcurl-d.a)





