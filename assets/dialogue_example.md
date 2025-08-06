# Village Elder Dialogue

**NPC:** Village Elder  
**Starting Node:** 0

---

## Node 0: Initial Greeting
**Speaker:** Village Elder  
**Text:** Welcome, traveler! I haven't seen you around these parts before. What brings you to our humble village?

### Choices:
1. **"I'm just passing through."** → Node 1
2. **"I'm looking for work."** → Node 2 `[Action: set_flag:looking_for_work]`
3. **"I seek the ancient ruins."** → Node 3 `[Condition: has_flag:heard_about_ruins]`

---

## Node 1: Passing Through
**Speaker:** Village Elder  
**Text:** Safe travels then, young one. Be careful on the roads - there have been strange creatures about lately.

### Choices:
1. **"Thank you for the warning."** → END `[Action: add_experience:10]`
2. **"What kind of creatures?"** → Node 4

---

## Node 2: Looking for Work
**Speaker:** Village Elder  
**Text:** Ah, a worker! We could use someone like you. The old mine has been overrun with goblins. Clear it out and I'll pay you 100 gold.

**On Enter:** `play_sound:quest_available`

### Choices:
1. **"I accept the job."** → END `[Action: start_quest:clear_mine]`
2. **"That sounds too dangerous."** → Node 1

---

## Node 3: Ancient Ruins
**Speaker:** Village Elder  
**Text:** The ruins... yes, I know of them. But they are cursed, young one. Many have entered, few have returned. Are you certain this is your path?

**On Enter:** `play_music:mysterious_theme`

### Choices:
1. **"I must try. It's my destiny."** → Node 5
2. **"Perhaps you're right. I should reconsider."** → Node 1

---

## Node 4: About the Creatures
**Speaker:** Village Elder  
**Text:** Shadow wolves, mostly. They appear at night and seem to be drawn to something in the old ruins. Some say they guard ancient treasures.

### Choices:
1. **"Interesting. Thank you for the information."** → END `[Action: set_flag:knows_about_shadow_wolves]`

---

## Node 5: Receiving the Key
**Speaker:** Village Elder  
**Text:** Very well. Take this ancient key - it may help you in the ruins. But remember, I warned you of the dangers.

**On Exit:** `set_flag:received_ancient_key`  
**End Node:** true

### Choices:
1. **"Thank you, elder. I won't forget your kindness."** → END `[Action: give_item:ancient_key]`

---

## Special Actions Reference

- `set_flag:flag_name` - Sets a game flag
- `has_flag:flag_name` - Condition: check if flag exists
- `add_experience:amount` - Give player experience
- `start_quest:quest_id` - Start a quest
- `give_item:item_id` - Give item to player
- `play_sound:sound_name` - Play sound effect
- `play_music:music_name` - Play background music
