class SpriteSheet {
private:
    QPixmap spriteSheet;
    int frameWidth;
    int frameHeight;
    int framesPerRow;
    int totalFrames;

public:
    SpriteSheet(const QString& imagePath, int frameW, int frameH, int framesRow, int total)
        : frameWidth(frameW), frameHeight(frameH), framesPerRow(framesRow), totalFrames(total) {
        spriteSheet.load(imagePath);
    }
    
    QPixmap getFrame(int frameIndex) {
        if (frameIndex >= totalFrames) return QPixmap();
        
        int row = frameIndex / framesPerRow;
        int col = frameIndex % framesPerRow;
        
        QRect frameRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight);
        return spriteSheet.copy(frameRect);
    }
};