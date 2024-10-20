#include <DirectXMath.h>
#include "application.h"

using namespace DirectX;

cpu_raytracer::application::application()
{
}

cpu_raytracer::application::~application()
{
}

const std::vector<BYTE>& cpu_raytracer::application::frame(int width, int height, float deltaTime)
{
    if (width < 8)
        width = 8;

    if (height < 8)
        height = 8;

    render(width, height);
    return m_data;
}

bool intersects_sphere(const XMVECTOR& rayOrigin, const XMVECTOR& rayDir, const XMVECTOR& sphereCenter, float radius, float& t)
{
    XMVECTOR oc = XMVectorSubtract(rayOrigin, sphereCenter);
    float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
    float b = 2.0f * XMVectorGetX(XMVector3Dot(oc, rayDir));
    float c = XMVectorGetX(XMVector3Dot(oc, oc)) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0)
    {
        t = (-b - sqrtf(discriminant)) / (2.0f * a);
        return true;
    }
    return false;
}

void cpu_raytracer::application::render(int width, int height)
{
    m_data.resize(width * height * 4);  // Assuming RGBA (4 bytes per pixel)

    // Compute aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // Sphere definition
    float sphereRadius = 0.5f;
    XMVECTOR sphereCenter = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);  // Sphere centered at z = -1
    XMVECTOR lightDir = XMVector3Normalize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));  // Light direction

    // Ray origin (camera position)
    XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    // Loop through every pixel on the screen
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // Map pixel coordinates to normalized device coordinates (NDC)
            float u = (2.0f * x / width - 1.0f) * aspectRatio;  // Adjust horizontal (u) by aspect ratio
            float v = (2.0f * y / height - 1.0f);
            v = -v;  // Invert Y for image coordinates

            // Ray direction (from camera to screen space)
            XMVECTOR rayDir = XMVector3Normalize(XMVectorSet(u, v, -1.0f, 0.0f));

            // Check if the ray intersects the sphere
            float t;
            if (intersects_sphere(rayOrigin, rayDir, sphereCenter, sphereRadius, t))
            {
                // Compute hit point and lighting (diffuse shading)
                XMVECTOR hitPoint = XMVectorAdd(rayOrigin, XMVectorScale(rayDir, t));
                XMVECTOR normal = XMVector3Normalize(XMVectorSubtract(hitPoint, sphereCenter));
                float lightIntensity = max(0.0f, XMVectorGetX(XMVector3Dot(normal, lightDir)));

                // Scale intensity to BYTE range [0, 255]
                int index = (y * width + x) * 4;
                m_data[index + 0] = static_cast<BYTE>(lightIntensity * 255);  // Red
                m_data[index + 1] = static_cast<BYTE>(lightIntensity * 255);  // Green
                m_data[index + 2] = static_cast<BYTE>(lightIntensity * 255);  // Blue
                m_data[index + 3] = 255;                                       // Alpha (fully opaque)
            }
            else
            {
                // Background color (black)
                int index = (y * width + x) * 4;
                m_data[index + 0] = 0;    // Red
                m_data[index + 1] = 0;    // Green
                m_data[index + 2] = 0;    // Blue
                m_data[index + 3] = 255;  // Alpha (fully opaque)
            }
        }
    }
}
