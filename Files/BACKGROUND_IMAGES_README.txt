==========================================
BACKGROUND IMAGES SETUP GUIDE
==========================================

Starting screens (MainScreen aur SelectionScreen) mein background images add karne ke liye:

📁 IMAGE PLACEMENT LOCATIONS:
-----------------------------

Option 1: Project root folder mein "backgrounds" folder banayein:
   Files/
   ├── backgrounds/
   │   ├── main_bg.jpg (or .png)      <- MainScreen ke liye
   │   └── selection_bg.jpg (or .png)  <- SelectionScreen ke liye
   ├── main.cpp
   ├── mainscreen.cpp
   └── selectionscreen.cpp

Option 2: Project root folder mein "images" folder banayein:
   Files/
   ├── images/
   │   ├── main_bg.jpg (or .png)
   │   └── selection_bg.jpg (or .png)
   └── ...

Option 3: Direct project root folder mein:
   Files/
   ├── main_bg.jpg (or .png)
   ├── selection_bg.jpg (or .png)
   └── ...


📸 RECOMMENDED IMAGE SPECIFICATIONS:
-----------------------------------
- Resolution: 1920x1080 (minimum) ya higher
- Format: JPG (smaller size) ya PNG (better quality)
- Aspect Ratio: 16:9 (widescreen)
- File Size: 500KB - 2MB (optimized)


🎨 IMAGE SUGGESTIONS (Orange Theme):
-------------------------------------
1. Dark abstract orange/red gradient backgrounds
2. Tech/cyberpunk style with orange accents
3. Minimalist geometric patterns in orange tones
4. Abstract fire/orange energy patterns
5. Dark space/galaxy with orange stars


🔗 FREE IMAGE RESOURCES:
------------------------
1. Unsplash: https://unsplash.com/s/photos/orange-gradient-dark
   Search: "orange gradient", "dark orange", "abstract orange"

2. Pexels: https://www.pexels.com/search/orange%20background/
   Search: "orange background", "dark orange"

3. Pixabay: https://pixabay.com/images/search/orange%20background/
   Search: "orange background", "abstract orange"

4. Freepik: https://www.freepik.com/search?format=search&query=orange+dark+background
   Search: "orange dark background", "abstract orange"

5. Gradient Backgrounds:
   - https://cssgradient.io/
   - https://uigradients.com/


💡 TIPS:
--------
- Image dark honi chahiye taaki text readable rahe
- Code automatically dark overlay (100 opacity) add karta hai
- Agar image nahi milegi, gradient background automatically use hoga
- Image ka aspect ratio maintain hoga (no stretching)


✅ TESTING:
-----------
1. Image ko specified location par rakhein
2. Application run karein
3. Console output check karein - "Background image loaded from: ..." dikhega
4. Agar image nahi mili, "No background image found" message aayega


📝 CURRENT IMAGE PATHS (Code automatically check karega):
---------------------------------------------------------
MainScreen:
- backgrounds/main_bg.jpg
- backgrounds/main_bg.png
- images/main_bg.jpg
- images/main_bg.png
- main_bg.jpg
- main_bg.png

SelectionScreen:
- backgrounds/selection_bg.jpg
- backgrounds/selection_bg.png
- images/selection_bg.jpg
- images/selection_bg.png
- selection_bg.jpg
- selection_bg.png
- backgrounds/main_bg.jpg (fallback)
- backgrounds/main_bg.png (fallback)


🎯 QUICK START:
---------------
1. Project folder mein "backgrounds" folder banayein
2. Apni images ko "main_bg.jpg" aur "selection_bg.jpg" naam se save karein
3. Application run karein - images automatically load ho jayengi!


==========================================

