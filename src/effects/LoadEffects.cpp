/**********************************************************************

  Audacity: A Digital Audio Editor

  LoadEffects.cpp

  Dominic Mazzoni

**********************************************************************/

#include "Effect.h"

#include "Amplify.h"
#include "BassBoost.h"
#include "Compressor.h"
#include "Echo.h"
#include "Fade.h"
#include "Filter.h"
#include "NoiseRemoval.h"
#include "Phaser.h"
#include "Wahwah.h"
#include "WaveletDenoise.h"

#ifdef __WXMAC__
#include "VST/LoadVSTMac.h"
#endif

#ifdef __WXMSW__
#include "VST/LoadVSTWin.h"
#endif

#ifdef __WXGTK__
#include "ladspa/LoadLadspa.h"
#endif

void LoadEffects()
{
   Effect::RegisterEffect(new EffectAmplify());
   Effect::RegisterEffect(new EffectBassBoost());
   Effect::RegisterEffect(new EffectCompressor());
   Effect::RegisterEffect(new EffectEcho());
   Effect::RegisterEffect(new EffectFadeIn());
   Effect::RegisterEffect(new EffectFadeOut());
   Effect::RegisterEffect(new EffectFilter());
   Effect::RegisterEffect(new EffectNoiseRemoval());
   Effect::RegisterEffect(new EffectPhaser());
   Effect::RegisterEffect(new EffectWahwah());
   Effect::RegisterEffect(new EffectWaveletDenoise());

#if defined(__WXMAC__) || defined(__WXMSW__)
   LoadVSTPlugins();
#endif

#if defined(__WXGTK__)
   LoadLadspaPlugins();
#endif

   // TODO: sort
}
