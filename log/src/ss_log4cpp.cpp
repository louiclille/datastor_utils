#include "log4cpp/ss_log4cpp.h"

ss_log4cpp &logger = ss_log4cpp::getInstance();

ss_log4cpp *ss_log4cpp::ss_log_ = NULL;

ss_log4cpp &ss_log4cpp::getInstance()
{
    if ( ss_log_ == NULL )
    {
        ss_log_ = new ss_log4cpp;
    }
    return *ss_log_;
}

void ss_log4cpp::destroy()
{
    if ( ss_log_ )
    {
        ss_log_->root_category.info("ss_log4cpp destroy");
        ss_log_->error_category.shutdown();
        ss_log_->root_category.shutdown();
        delete ss_log_;
    }
}

ss_log4cpp::ss_log4cpp() : root_category(log4cpp::Category::getRoot()),
                           error_category(log4cpp::Category::getInstance(std::string("error"))),
                           warn_category(log4cpp::Category::getInstance(std::string("warn"))),
                           info_category(log4cpp::Category::getInstance(std::string("info"))),
                           debug_category(log4cpp::Category::getInstance(std::string("debug")))
{
    log4cpp::PatternLayout *root_layout = new log4cpp::PatternLayout();
    root_layout->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S.%l} - %p] : %m%n");

    log4cpp::RollingFileAppender *roll_appender_all = new log4cpp::RollingFileAppender(
            "roll_appender", "./ss_log.log", 1024*1024*100, 3);
    roll_appender_all->setLayout(root_layout);


    log4cpp::PatternLayout *layout_error = new log4cpp::PatternLayout();
    layout_error->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S.%l} - %p] : %m%n");

    log4cpp::RollingFileAppender *roll_appender_error = new log4cpp::RollingFileAppender(
            "roll_appender_error", "./ss_log_error.log", 1024*1024*100, 3);
    roll_appender_error->setLayout(layout_error);


    log4cpp::PatternLayout *layout_warn = new log4cpp::PatternLayout();
    layout_warn->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S.%l} - %p] : %m%n");

    log4cpp::RollingFileAppender *roll_appender_warn = new log4cpp::RollingFileAppender(
            "roll_appender_error", "./ss_log_warn.log", 1024*1024*100, 3);
    roll_appender_warn->setLayout(layout_warn);


    log4cpp::PatternLayout *layout_info = new log4cpp::PatternLayout();
    layout_info->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S.%l} - %p] : %m%n");

    log4cpp::RollingFileAppender *roll_appender_info = new log4cpp::RollingFileAppender(
            "roll_appender_error", "./ss_log_info.log", 1024*1024*100, 3);
    roll_appender_info->setLayout(layout_info);

    log4cpp::PatternLayout *layout_debug = new log4cpp::PatternLayout();
    layout_debug->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S.%l} - %p] : %m%n");

    log4cpp::RollingFileAppender *roll_appender_debug = new log4cpp::RollingFileAppender(
            "roll_appender_error", "./ss_log_debug.log", 1024*1024*100, 3);
    roll_appender_debug->setLayout(layout_debug);


    root_category.setPriority(log4cpp::Priority::DEBUG);
    root_category.addAppender(roll_appender_all);

    error_category.setPriority(log4cpp::Priority::ERROR);
    error_category.setAppender(roll_appender_error);

    warn_category.setPriority(log4cpp::Priority::WARN);
    warn_category.setAppender(roll_appender_warn);

    info_category.setPriority(log4cpp::Priority::INFO);
    info_category.setAppender(roll_appender_info);

    debug_category.setPriority(log4cpp::Priority::DEBUG);
    debug_category.setAppender(roll_appender_debug);
}

ss_log4cpp::~ss_log4cpp()
{
    destroy();
}

void ss_log4cpp::error(std::string msg)
{
    error_category.error(msg);
}

void ss_log4cpp::warn(std::string msg)
{
    warn_category.warn(msg);
}

void ss_log4cpp::info(std::string msg)
{
    info_category.info(msg);
}

void ss_log4cpp::debug(std::string msg)
{
    debug_category.debug(msg);
}