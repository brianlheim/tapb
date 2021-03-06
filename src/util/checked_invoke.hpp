#pragma once

#include "util/simple_options.hpp"

#include <array>
#include <iostream>
#include <tuple>

// Expected signature of lambda: bool(std::string x N)
template <typename F, size_t N>
[[nodiscard]] static int checked_invoke( simple_options::options & opts,
                                         const std::array<const char *, N> & arg_names,
                                         F && lambda ) noexcept {
    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    bool has_all = std::all_of( begin( arg_names ), end( arg_names ),
                                [&opts]( auto * name ) { return opts.has( name ); } );
    if ( has_all ) {
        std::array<std::string, N> args;
        for ( size_t i = 0; i < N; ++i ) {
            args[i] = opts[arg_names[i]].template as<std::string>();
        }
        if ( apply( std::forward<F>( lambda ), args ) ) {
            return 0;
        } else {
            std::cout << "Operation failed." << std::endl;
            return 2;
        }
    } else {
        std::cout << opts;
        return 1;
    }
}

template <typename F>
[[nodiscard]] static int checked_invoke_in_out( simple_options::options & opts,
                                                F && lambda ) noexcept {
    return checked_invoke( opts, std::array{ "input", "output" }, std::forward<F>( lambda ) );
}

template <typename F>
[[nodiscard]] static int checked_invoke_in_out_bkpts( simple_options::options & opts,
                                                      F && lambda ) noexcept {
    return checked_invoke( opts, std::array{ "input", "output", "breakpoints" },
                           std::forward<F>( lambda ) );
}
