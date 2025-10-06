#ifndef SHOOTFAST_SERVER_APPLICATION_HPP
#define SHOOTFAST_SERVER_APPLICATION_HPP

namespace ShootFast::Server
{
    class ServerApplication final
    {

    public:

        ServerApplication(const ServerApplication&) = delete;
        ServerApplication(ServerApplication&&) = delete;
        ServerApplication& operator=(const ServerApplication&) = delete;
        ServerApplication& operator=(ServerApplication&&) = delete;

        ~ServerApplication();

        void Preinitialize();
        void Initialize();

        [[nodiscard]]
        bool IsRunning() const;

        void Update();

        static ServerApplication& GetInstance();

    private:

        ServerApplication() = default;
        
    };
}

#endif