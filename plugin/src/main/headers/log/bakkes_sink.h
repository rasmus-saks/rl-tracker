#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <iostream>

template<typename Mutex>
class bakkes_sink : public spdlog::sinks::base_sink <Mutex>
{
public:
    bakkes_sink(std::shared_ptr<CVarManagerWrapper> cvarManager) : cvarManager(cvarManager) {}
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        cvarManager->log(fmt::to_string(formatted));
    }

    void flush_() override
    {
        std::cout << std::flush;
    }
private:
    std::shared_ptr<CVarManagerWrapper> cvarManager;
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using bakkes_sink_mt = bakkes_sink<std::mutex>;
using bakkes_sink_st = bakkes_sink<spdlog::details::null_mutex>;
