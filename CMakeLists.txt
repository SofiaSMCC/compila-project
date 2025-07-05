cmake_minimum_required(VERSION 3.28.1)
project(Compila_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compila_Project
        token.cpp
        scanner.cpp
        main.cpp
        exp.cpp
        parser.cpp
        visitor.cpp
        imp_value.cpp
        environment.h
        type_visitor.cpp
        eval_visitor.cpp
        codegen.cpp)

# Habilitar testing
enable_testing()

# Buscar todos los archivos .txt en las subcarpetas de test
file(GLOB_RECURSE test_files CONFIGURE_DEPENDS "cmake-build-debug/test/*/*.txt")

# Verificar si encontramos archivos de test
if(test_files)
    list(LENGTH test_files num_tests)
    message(STATUS "Encontrados ${num_tests} archivos de test")

    # Mostrar las carpetas que encontramos
    foreach(test_file ${test_files})
        get_filename_component(test_dir ${test_file} DIRECTORY)
        get_filename_component(test_dir_name ${test_dir} NAME)
        message(STATUS "Test encontrado: ${test_dir_name}/${test_file}")
    endforeach()

    # Crear tests para cada archivo .txt
    foreach(test_file ${test_files})
        # Obtener el nombre del archivo sin extensión
        get_filename_component(test_name ${test_file} NAME_WE)

        # Obtener el nombre de la carpeta padre (array, cond_stm, etc.)
        get_filename_component(test_dir ${test_file} DIRECTORY)
        get_filename_component(test_category ${test_dir} NAME)

        # Crear nombre único para el test: categoria_archivo
        set(test_label "${test_category}_${test_name}")

        # Registrar el test
        add_test(
                NAME "${test_label}"
                COMMAND $<TARGET_FILE:Compila_Project> "${test_file}"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

        # Configurar propiedades del test
        set_tests_properties("${test_label}" PROPERTIES
                TIMEOUT 30
                LABELS "${test_category}"
        )

        message(STATUS "Test registrado: ${test_label} -> ${test_file}")
    endforeach()

else()
    message(WARNING "No se encontraron archivos .txt en las subcarpetas de test/")
endif()

# Target personalizado para ejecutar todos los tests
add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
        DEPENDS Compila_Project
        COMMENT "Ejecutando todos los tests"
)

# Targets para ejecutar tests por categoría
set(categories "array" "cond_stm" "exp" "func" "strings" "var_decl")
foreach(category ${categories})
    add_custom_target(run_${category}_tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L ${category}
            DEPENDS Compila_Project
            COMMENT "Ejecutando tests de ${category}"
    )
endforeach()

# Información útil
message(STATUS "")
message(STATUS "=== INFORMACIÓN DE TESTS ===")
message(STATUS "Para ejecutar todos los tests: make run_all_tests")
message(STATUS "Para ejecutar tests por categoría: make run_array_tests, make run_cond_stm_tests, etc.")
message(STATUS "Para ejecutar tests manualmente: ctest --output-on-failure")
message(STATUS "Para ver todos los tests: ctest --show-only")
message(STATUS "===============================")