#ifndef SHOOTFAST_SETTINGS_HPP
#define SHOOTFAST_SETTINGS_HPP

namespace ShootFast::Independent::Core
{
    class Settings final
    {

    public:

        Settings(const Settings&) = delete;
        Settings(Settings&&) = delete;
        Settings& operator=(const Settings&) = delete;
        Settings& operator=(Settings&&) = delete;

#ifdef IS_SERVER_FLAG
        static constexpr bool IS_SERVER = true;
#else
        static constexpr bool IS_SERVER = false;
#endif

    private:

        Settings() = default;

    };
}

#endif