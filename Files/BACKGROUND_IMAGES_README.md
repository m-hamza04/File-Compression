# Background Images Setup Guide

## Image Links (Orange-themed Abstract Backgrounds)

### Unsplash (Free, High Quality):
1. **Orange Abstract Gradient**
   - Link: https://unsplash.com/photos/orange-gradient-abstract-background
   - Search: "orange abstract gradient" on Unsplash
   - Direct download: https://unsplash.com/s/photos/orange-abstract-gradient

2. **Orange Modern Background**
   - Link: https://unsplash.com/photos/orange-modern-background
   - Search: "orange modern background" on Unsplash

3. **Best Collection**
   - Unsplash Orange Backgrounds: https://unsplash.com/s/photos/orange-background
   - Unsplash Abstract Orange: https://unsplash.com/s/photos/abstract-orange

### Pexels (Free Stock Photos):
1. **Orange Abstract**
   - Link: https://www.pexels.com/search/orange%20abstract/
   - Direct: https://www.pexels.com/search/orange%20background/

2. **Orange Gradient**
   - Link: https://www.pexels.com/search/orange%20gradient/

### Recommended Image Specs:
- **Size**: 1920x1080 (Full HD) or higher
- **Format**: JPG or PNG
- **Theme**: Orange, warm colors, abstract, modern
- **Style**: Dark orange gradients, subtle patterns, modern tech vibe

## How to Add Images:

1. **Create a folder** in your project directory:
   ```
   Files/
   ├── backgrounds/
   │   ├── main_bg.jpg      (For MainScreen)
   │   └── selection_bg.jpg (For SelectionScreen)
   ```

2. **Download images** from the links above and save them with these exact names:
   - `main_bg.jpg` - For the starting screen
   - `selection_bg.jpg` - For the selection screen

3. **Place images** in one of these locations (code will try all):
   - `Files/backgrounds/main_bg.jpg`
   - `Files/backgrounds/selection_bg.png`
   - `Files/images/main_bg.jpg`
   - `Files/main_bg.jpg` (root directory)

4. **Alternative**: If images not found, the app will automatically use the orange gradient background.

## Quick Download Links:

### Option 1: Unsplash Direct
- Visit: https://unsplash.com/s/photos/orange-abstract
- Filter by: Orientation (Landscape), Colors (Orange)
- Download any image you like
- Rename to `main_bg.jpg` and `selection_bg.jpg`

### Option 2: Pexels Direct  
- Visit: https://www.pexels.com/search/orange%20gradient/
- Download images
- Rename and place in `backgrounds/` folder

### Option 3: Use These Specific Images
1. **Orange Abstract Pattern**: 
   - https://images.unsplash.com/photo-1557683316-973673baf926 (Unsplash)

2. **Orange Gradient Background**:
   - https://images.unsplash.com/photo-1579546929518-9e396f3cc809 (Unsplash)

3. **Modern Orange Tech Background**:
   - Search "orange tech background" on Pexels or Unsplash

## Notes:
- Images should be at least 1920x1080 for best quality
- The code automatically adds a dark overlay (100 opacity) for text readability
- Images are scaled to fill the screen while maintaining aspect ratio
- If image not found, gradient background will be used automatically

