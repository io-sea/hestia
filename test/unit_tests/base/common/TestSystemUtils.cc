#include <catch2/catch_all.hpp>

#include "ErrorUtils.h"
#include "SystemUtils.h"
#include "TestUtils.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

TEST_CASE("Test SystemUtils - Mac address", "[common]")
{

    //(void* handle, const std::string& module_name, const std::string& name);
    // auto [status_ldsmb, symbol] = hestia::SystemUtils::load_symbol(handle,
    // name, "Name"); REQUIRE(status_ldsmb.ok());

    auto [status_adr, address] = hestia::SystemUtils::get_mac_address();
    std::cout << "STATUS MAC ADDRESS OK: " << status_adr.ok() << std::endl;
    std::cout << "STATUS MAC ADDRESS CODE: " << status_adr.code() << std::endl;
    std::cout << "STATUS MAC ADDRESS MESSAGE: " << status_adr.message()
              << std::endl;
    std::cout << "STATUS MAC ADDRESS STR: " << status_adr.str() << std::endl;
    std::cout << "ADDRESS: " << address << std::endl;
    // REQUIRE(status_adr.ok());

    /*auto [status_hn, hostname] =  hestia::SystemUtils::get_hostname();
    std::cout<<"STATUS HOST NAME: "<< status_hn.ok()<<std::endl;
    std::cout<<"HOSTNAME: " << hostname<<std::endl;
    REQUIRE(status_hn.ok());*/

    // std::pair<OpStatus, void*> SystemUtils::load_module(const std::string&
    // path) std::string path="test"; auto [status_ldmod, handle_mod] =
    // hestia::SystemUtils::load_module(path); REQUIRE(status_ldmod.ok());

    //(void* handle, const std::string& name)
    // const auto status_clmod = hestia::SystemUtils::close_module(handle,
    // name); REQUIRE(status_clmod.ok());

    auto test_output_dir = TestUtils::get_test_output_dir();
    const std::filesystem::path path{TestUtils::get_test_output_dir()};
    std::ofstream f_out;
    f_out.open(path / "test.txt");
    f_out << "Test";
    f_out.close();

    /*static constexpr std::size_t num_fdescriptors = 3;
    hestia::handle_t fdescriptors[num_fdescriptors]{0, 1, 2};
    const auto status_opp = hestia::SystemUtils::open_pipe(fdescriptors);
    std::cout<<"STATUS OPEN PIPE: "<< status_opp.ok()<<std::endl;
    REQUIRE(status_opp.ok());*/

    /*hestia::handle_t fdescriptor = 1;
    const auto status_dcl = hestia::SystemUtils::do_close(fdescriptor);
    std::cout<<"STATUS DO CLOSE: "<< status_opp.ok()<<std::endl;
    REQUIRE(status_dcl.ok());*/

    //(handle_t file_descriptors[], std::size_t count);
    // const auto status_clfs = hestia::SystemUtils::close_files(m_descriptors,
    // count); REQUIRE(status_clfs.ok());

    // static void close_standard_file_descriptors();
    // hestia::SystemUtils::close_standard_file_descriptors();
}
