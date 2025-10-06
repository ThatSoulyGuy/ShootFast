#ifndef SHOOTFAST_RENDER_COMPONENTS_HPP
#define SHOOTFAST_RENDER_COMPONENTS_HPP

#include <concepts>

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;

    template <typename T>
    concept HasHandle = requires { typename T::Handle; };

    template <typename T>
    concept Creatable = requires(const T& specification, ClientRenderContext& context)
    {
        { T::Create(context, specification) } -> std::same_as<typename T::Handle>;
    };

    template <typename T>
    concept Uploadable = requires(const T& specification, ClientRenderContext& context)
    {
        { T::Upload(context, specification) } -> std::same_as<typename T::Handle>;
    };

    template <typename T>
    concept Destroyable = requires(ClientRenderContext& context, typename T::Handle handle)
    {
        { T::Destroy(context, handle) } -> std::same_as<void>;
    };

    template <typename T> requires HasHandle<T>
    class RenderComponents final
    {

    public:

        using Handle = T::Handle;

        static Handle Create(ClientRenderContext& context, const T& specification) requires Creatable<T>
        {
            return T::Create(context, specification);
        }

        static Handle Upload(ClientRenderContext& context, const T& specification) requires Uploadable<T>
        {
            return T::Upload(context, specification);
        }

        static void Destroy(ClientRenderContext& context, Handle handle) requires Destroyable<T>
        {
            T::Destroy(context, handle);
        }
    };
}

#endif