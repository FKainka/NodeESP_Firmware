#ifndef NEOPIXEL_WRAPPER
#define  NEOPIXEL_WRAPPER

#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>  //https://github.com/Makuna/NeoPixelBus/wiki
#include <NeoPixelBrightnessBus.h>


#define KETTE

//#ifdef BAND
#define METHOD NeoEsp32BitBang800KbpsMethod //NeoEsp32BitBangWs2813Method  // NeoEsp32BitBang400KbpsMethod NeoEsp32BitBang800KbpsMethod
#define FEATURE NeoGrbFeature // NeoRgbFeature
#define NEOCLASS  NeoPixelBrightnessBus // NeoPixelBus
#define NUMMER_PIXEL 82
/*
// KETTE
#define METHOD NeoEsp32BitBangWs2813Method  // NeoEsp32BitBang400KbpsMethod
#define FEATURE NeoRgbFeature // NeoRgbFeature
#define NEOCLASS NeoPixelBrightnessBus // NeoPixelBus
#define NUMMER_PIXEL 40
/*
#elseif RINGE
#define METHOD NeoEsp32BitBangWs2813Method  // NeoEsp32BitBang400KbpsMethod
#define FEATURE NeoGrbFeature // NeoRgbFeature
#define NEOCLASS NeoPixelBrightnessBus // NeoPixelBus
#define NUMMER_PIXEL 16
#endif
*/


enum NeoPixelType
{
  NeoPixelType_None = 0,
  NeoPixelType_Rgb  = 1,
  NeoPixelType_Grb  = 2,
  NeoPixelType_Rgbw = 3,
  NeoPixelType_Grbw = 4,
  NeoPixelType_End  = 5
};


typedef struct {
        uint16_t number_pixel = 82;
        uint8_t max_brightness = 100;
        uint8_t brightness = 100;
        uint8_t pin = 14;
        int type = NeoPixelType_Grb ;
}neopixel_set_s;

neopixel_set_s neopixel_set;


#define Is_RGBW(l) ( l==NeoPixelType_Rgbw || l==NeoPixelType_Grbw )

class NeoPixelWrapper
{
public:
  NeoPixelWrapper() :
      // initialize each member to null
      _pRgb(NULL),
      _pGrb(NULL),
      _pRgbw(NULL),
      _pGrbw(NULL),
      _type(NeoPixelType_None)
  {

  }

  ~NeoPixelWrapper()
  {
    cleanup();
  }

  void Begin(NeoPixelType type, uint16_t countPixels, uint8_t pin)
  {
    cleanup();
    _type = type;

    switch (_type) {

      case NeoPixelType_Rgb:
        _pRgb = new NEOCLASS<NeoRgbFeature,METHOD>(countPixels, pin);
        _pRgb->Begin();
      break;

      case NeoPixelType_Grb:
        _pGrb = new NEOCLASS<NeoGrbFeature,METHOD>(countPixels, pin);
        _pGrb->Begin();
      break;

      case NeoPixelType_Rgbw:
        _pRgbw = new NEOCLASS<NeoRgbwFeature,METHOD>(countPixels, pin);
        _pRgbw->Begin();
      break;

      case NeoPixelType_Grbw:
        _pGrbw = new NEOCLASS<NeoGrbwFeature,METHOD>(countPixels, pin);
        _pGrbw->Begin();
      break;
    }
  }

  void Show()
  {
    portDISABLE_INTERRUPTS(); //Interrupt disable because timing issues und esp32
    switch (_type) {
      case NeoPixelType_Rgb:  _pRgb->Show();   break;
      case NeoPixelType_Grb:  _pGrb->Show();   break;
      case NeoPixelType_Rgbw: _pRgbw->Show();  break;
      case NeoPixelType_Grbw: _pGrbw->Show();  break;
    }
    portENABLE_INTERRUPTS();
  }

  bool CanShow() const
  {
    switch (_type) {
      case NeoPixelType_Rgb:  _pRgb->CanShow();  break;
      case NeoPixelType_Grb:  _pGrb->CanShow();  break;
      case NeoPixelType_Rgbw: _pRgbw->CanShow(); break;
      case NeoPixelType_Grbw: _pGrbw->CanShow(); break;
    }
  }

  // replicate all the calls like the above
    bool IsDirty() const
    {
    }

    void Dirty()
    {
    }

    void ResetDirty()
    {
    }

    uint8_t* Pixels() const
    {
    }

    size_t PixelsSize() const
    {
    }

    size_t PixelSize() const
    {
    }

    uint16_t PixelCount() const
    {
    }

    void SetBrightness(uint8_t brightness)
    {
      switch (_type) {
        case NeoPixelType_Rgb: _pRgb->SetBrightness(brightness);   break;
        case NeoPixelType_Grb: _pGrb->SetBrightness(brightness);   break;
        case NeoPixelType_Rgbw:_pRgbw->SetBrightness(brightness);  break;
        case NeoPixelType_Grbw:_pGrbw->SetBrightness(brightness);  break;
      }
    }

    void SetPixelColor(uint16_t indexPixel, RgbColor color)
    {
      switch (_type) {
        case NeoPixelType_Rgb: _pRgb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Grb: _pGrb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Rgbw:_pRgbw->SetPixelColor(indexPixel, color);  break;
        case NeoPixelType_Grbw:_pGrbw->SetPixelColor(indexPixel, color);  break;
      }
    }

    void SetPixelColor(uint16_t indexPixel, HtmlColor color)
    {
      switch (_type) {
        case NeoPixelType_Rgb: _pRgb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Grb: _pGrb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Rgbw:_pRgbw->SetPixelColor(indexPixel, color);  break;
        case NeoPixelType_Grbw:_pGrbw->SetPixelColor(indexPixel, color);  break;
      }
    }


    void SetPixelColor(uint16_t indexPixel, HslColor color)
    {
      switch (_type) {
        case NeoPixelType_Rgb: _pRgb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Grb: _pGrb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Rgbw:_pRgbw->SetPixelColor(indexPixel, color);  break;
        case NeoPixelType_Grbw:_pGrbw->SetPixelColor(indexPixel, color);  break;
      }
    }

    void SetPixelColor(uint16_t indexPixel, RgbwColor color)
    {
      switch (_type) {
        case NeoPixelType_Rgb: /* doesn't support it so we don't call it*/  break;
        case NeoPixelType_Grb:  _pGrbw->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Rgbw: _pRgbw->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Grbw: _pGrbw->SetPixelColor(indexPixel, color);   break;
      }
    }

    RgbColor GetPixelColor(uint16_t indexPixel) const
    {
      switch (_type) {
        case NeoPixelType_Rgb:  return _pRgb->GetPixelColor(indexPixel);     break;
        case NeoPixelType_Grb:  return _pGrb->GetPixelColor(indexPixel);     break;
        case NeoPixelType_Rgbw: /*doesn't support it so we don't return it*/ break;
        case NeoPixelType_Grbw: /*doesn't support it so we don't return it*/ break;
      }
      return 0;
    }

// NOTE:  Due to feature differences, some support RGBW but the method name
// here needs to be unique, thus GetPixeColorRgbw
    RgbwColor GetPixelColorRgbw(uint16_t indexPixel) const
    {
      switch (_type) {
        case NeoPixelType_Rgb:  return _pRgb->GetPixelColor(indexPixel);  break;
        case NeoPixelType_Grb:  return _pGrb->GetPixelColor(indexPixel);  break;
        case NeoPixelType_Rgbw: return _pRgbw->GetPixelColor(indexPixel); break;
        case NeoPixelType_Grbw: return _pGrbw->GetPixelColor(indexPixel); break;
      }
      return 0;
    }

    void ClearTo(RgbColor color)
    {
    }

    void ClearTo(RgbwColor color)
    {
    }

    void ClearTo(RgbColor color, uint16_t first, uint16_t last)
    {
    }

    void ClearTo(RgbwColor color, uint16_t first, uint16_t last)
    {
    }

    void RotateLeft(uint16_t rotationCount)
    {
    }

    void RotateLeft(uint16_t rotationCount, uint16_t first, uint16_t last)
    {
    }

    void ShiftLeft(uint16_t shiftCount)
    {
    }

    void ShiftLeft(uint16_t shiftCount, uint16_t first, uint16_t last)
    {
    }

    void RotateRight(uint16_t rotationCount)
    {
    }

    void RotateRight(uint16_t rotationCount, uint16_t first, uint16_t last)
    {
    }

    void ShiftRight(uint16_t shiftCount)
    {
    }

    void ShiftRight(uint16_t shiftCount, uint16_t first, uint16_t last)
    {
    }

private:
  NeoPixelType _type;

  // have a member for every possible type
  NEOCLASS<NeoRgbFeature,METHOD>*  _pRgb;
  NEOCLASS<NeoGrbFeature,METHOD>*  _pGrb;
  NEOCLASS<NeoRgbwFeature,METHOD>* _pRgbw;
  NEOCLASS<NeoGrbwFeature,METHOD>* _pGrbw;

  void cleanup()
  {
    switch (_type) {
      case NeoPixelType_Rgb:  delete _pRgb ; _pRgb  = NULL; break;
      case NeoPixelType_Grb:  delete _pGrb ; _pGrb  = NULL; break;
      case NeoPixelType_Rgbw: delete _pRgbw; _pRgbw = NULL; break;
      case NeoPixelType_Grbw: delete _pGrbw; _pGrbw = NULL; break;
    }
  }
};

#endif
