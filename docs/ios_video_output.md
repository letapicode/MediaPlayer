# iOS Metal Video Output

This short guide demonstrates how to connect `MetalVideoOutput` from the core engine to an `MTKView`.

## Attaching to `MTKView`

```objective-c
#import <MetalKit/MetalKit.h>
#import "mediaplayer/MediaPlayer.h"
#import "mediaplayer/MetalVideoOutput.h"

using namespace mediaplayer;

- (void)setupPlayerWithView:(MTKView *)view {
    auto player = std::make_unique<MediaPlayer>();
    auto output = std::make_unique<MetalVideoOutput>();

    output->init(view.drawableSize.width, view.drawableSize.height);
    view.layer = (__bridge CAMetalLayer *)output->layer();

    player->setVideoOutput(std::move(output));
    player->open("movie.mp4");
    player->play();
}
```

The view's `CAMetalLayer` is supplied by `MetalVideoOutput`, which renders each decoded frame directly into the layer attached to the `MTKView`.
