// Sets the macOS Dock icon at runtime (ImageIO reads .ico fine).
#import <AppKit/AppKit.h>

void SetMacDockIcon(const char *path)
{
    @autoreleasepool
    {
        NSImage *img = [[NSImage alloc]
            initWithContentsOfFile:[NSString stringWithUTF8String:path]];
        if (img)
            [NSApp setApplicationIconImage:img];
    }
}
