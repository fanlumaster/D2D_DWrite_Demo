#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dwmapi.lib")

ID2D1Factory *pD2DFactory = nullptr;
ID2D1HwndRenderTarget *pRenderTarget = nullptr;
ID2D1SolidColorBrush *pBrush = nullptr;
IDWriteFactory *pDWriteFactory = nullptr;
IDWriteTextFormat *pTextFormat = nullptr;

const wchar_t *CUSTOM_FONT = L"Microsoft YaHei UI";
const wchar_t *CUSTOM_LOCALE = L"zh-CN";

void EnableDarkMode(HWND hwnd)
{
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
}

void CreateD2DResources(HWND hwnd)
{
    if (!pD2DFactory)
    {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
    }

    if (!pRenderTarget)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &pRenderTarget);

        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
    }

    if (!pDWriteFactory)
    {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&pDWriteFactory));
    }

    if (!pTextFormat)
    {
        pDWriteFactory->CreateTextFormat(CUSTOM_FONT, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, CUSTOM_LOCALE, &pTextFormat);
    }
}

void ReleaseD2DResources()
{
    if (pBrush)
        pBrush->Release();
    if (pRenderTarget)
        pRenderTarget->Release();
    if (pD2DFactory)
        pD2DFactory->Release();
    if (pTextFormat)
        pTextFormat->Release();
    if (pDWriteFactory)
        pDWriteFactory->Release();

    pBrush = nullptr;
    pRenderTarget = nullptr;
    pD2DFactory = nullptr;
    pTextFormat = nullptr;
    pDWriteFactory = nullptr;
}

void DrawWithDirect2D(HWND hwnd)
{
    if (!pRenderTarget || !pBrush || !pTextFormat)
        return;

    pRenderTarget->BeginDraw();
    // RGB(25, 25, 25)
    pRenderTarget->Clear(D2D1::ColorF(25.0f / 255.0f, 25.0f / 255.0f, 25.0f / 255.0f));

    // draw rectangle
    D2D1_RECT_F rect = D2D1::RectF(50, 50, 500, 250);
    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
    pRenderTarget->FillRectangle(rect, pBrush);
    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);

    // draw text
    pRenderTarget->DrawText(L"Hello, Direct2D! 你好呀，Direct2D！", 30, pTextFormat, D2D1::RectF(60, 60, 500, 100), pBrush);
    pRenderTarget->DrawText(L"梁子，量子，毛笔", 8, pTextFormat, D2D1::RectF(60, 90, 500, 100), pBrush);

    HRESULT hr = pRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        ReleaseD2DResources();
        CreateD2DResources(hwnd);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
        DrawWithDirect2D(hwnd);
        break;
    case WM_SIZE:
        if (pRenderTarget)
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
            pRenderTarget->Resize(size);
        }
        break;
    case WM_MOUSEMOVE: {
        float x = (float)LOWORD(lParam);
        float y = (float)HIWORD(lParam);

        RECT rc;
        GetClientRect(hwnd, &rc);
        if (x >= rc.left && x <= rc.right && y >= rc.top && y <= rc.bottom)
        {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }
        break;
    }
    case WM_DESTROY:
        ReleaseD2DResources();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),      //
        CS_HREDRAW | CS_VREDRAW, //
        WndProc,                 //
        0,                       //
        0,                       //
        hInstance,               //
        nullptr,                 //
        nullptr,                 //
        nullptr,                 //
        nullptr,                 //
        L"Direct2DExample",      //
        nullptr                  //
    };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName,    //
                             L"Direct2D Example", //
                             WS_OVERLAPPEDWINDOW, //
                             CW_USEDEFAULT,       //
                             CW_USEDEFAULT,       //
                             838,                 //
                             600,                 //
                             nullptr,             //
                             nullptr,             //
                             hInstance,           //
                             nullptr              //
    );
    ShowWindow(hwnd, nCmdShow);
    EnableDarkMode(hwnd);

    CreateD2DResources(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}