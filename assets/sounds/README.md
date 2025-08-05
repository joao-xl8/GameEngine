# Sound Assets

This directory is for sound files. When you add sound files, update the assets.txt file to include them.

## Example sound files you can add:
- footstep.wav - Player walking sound
- jump.wav - Player jumping sound  
- collision.wav - Collision with walls/objects
- pickup.wav - Item pickup sound
- background_music.ogg - Background music

## Supported formats:
- WAV files for sound effects
- OGG files for music (recommended for longer audio)

## To add sounds:
1. Place sound files in this directory
2. Add entries to metadata/assets.txt like:
   ```
   Sound Footstep assets/sounds/footstep.wav
   Sound Jump assets/sounds/jump.wav
   Sound Collision assets/sounds/collision.wav
   ```
3. Uncomment the sound playing code in scene_play.cpp
