add_rules("mode.debug", "mode.release")

add_requires("glfw")
add_requires("glm")
add_requires("glad")


target("exec")
    set_kind("binary")
    set_languages("c++17")
    add_files("src/*.cpp", "src/imgui/*.cpp")
    add_includedirs("src/imgui")
    add_packages("glfw")
    add_packages("glm")
    add_packages("glad")
    add_links("stb")


    after_build(function (target) 
        os.cp("src/shader", target:targetdir())
        os.cp("src/img", target:targetdir())
    end)
