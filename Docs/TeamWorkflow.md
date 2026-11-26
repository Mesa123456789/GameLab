# คู่มือการทำงานร่วมกัน & กฎการตั้งชื่อไฟล์ — ScrubbyBooGame

> เอกสารกลางสำหรับทีมพัฒนา **ScrubbyBooGame** (Unreal Engine 5.6) เพื่อให้ **C++ Dev / Git Manager**, **Blueprint Dev** และ **Artist** ทำงานขนานกันได้โดยไฟล์ไม่ชนกัน และไม่ต้องปวดหัวกับการแก้ Merge Conflict บนไฟล์ Binary

---

## 1. บทบาทหน้าที่และขอบเขตไฟล์ (Roles & Ownership)

| ตำแหน่ง | เครื่องมือหลัก | โฟลเดอร์/ไฟล์ที่รับผิดชอบ | ใช้ Git หรือไม่? |
| :--- | :--- | :--- | :--- |
| **Multiplayer Core Dev & Git Manager** | Visual Studio 2022, Unreal Engine 5.6, AI, Git | `Source/ScrubbyBooGame/Core/`, `Multiplayer/`, `Shared/`, `Config/`, `.uproject`, `.Build.cs`, `Docs/`, และการ Merge เข้า `main` | **ใช้** (Git CLI / GitHub) |
| **Gameplay Dev (Blueprint + AI C++)** | Unreal Engine 5.6 Editor, GitHub Desktop, AI | `Content/ScrubbyBoo/` (BP, UI, Map, Material, Import Art) และโฟลเดอร์ระบบของตัวเองใต้ `Source/ScrubbyBooGame/Systems/<Feature>/` | **ใช้** (**GitHub Desktop** + Git LFS) |
| **Artist** | Blender / Maya / Substance / Photoshop / ฯลฯ | ไฟล์งานต้นฉบับ + ไฟล์ Export พร้อมเข้า Unreal บน Shared Cloud (Google Drive / OneDrive) | **ไม่ใช้** (ส่งงานผ่าน Cloud อย่างเดียว) |

> **ทำไม Artist ไม่ต้องใช้ Git?**
> 1. ไม่ต้องเสียเวลาเรียนรู้ Git/LFS หรือติดตั้ง Unreal Engine
> 2. ไฟล์ต้นฉบับขนาดใหญ่ (`.blend`, `.psd`, `.spp`) จะไม่กินพื้นที่ **GitHub Git LFS** (และไม่ทำให้ใน Repo เก็บไฟล์ซ้ำซ้อนทั้ง `.fbx`/`.png` และ `.uasset`)

---

## 2. โครงสร้างโฟลเดอร์ (Folder Structure)

### 2.1 โครงสร้างโค้ด C++ แยกตามระบบ (`Source/ScrubbyBooGame/`)
เพื่อให้ทั้ง 2 คน (รวมถึง AI ของแต่ละคน) เขียนโค้ด C++ ขนานกันได้โดยไฟล์ไม่ชนกัน ให้แยกโค้ดตามโครงสร้างนี้:

```text
Source/ScrubbyBooGame/
├── Core/             # [ไฟล์กลาง — ห้ามแก้โดยไม่แจ้ง] GameMode, GameState, PlayerState, PlayerController, Character base
├── Multiplayer/      # ระบบเน็ตเวิร์ก Session, Lobby, Replication Utilities (Multiplayer Dev ดูแล)
├── Shared/           # Interface (UInterface), GameplayTags, Struct/Enum กลางสำหรับให้ระบบคุยกัน (เพิ่มได้อย่างเดียว)
└── Systems/          # 1 ระบบใหญ่ = 1 โฟลเดอร์ แยกอิสระตามระบบเกม (สร้างเพิ่มตามฟีเจอร์ใหญ่ ไม่ต้องแยกย่อยทุกฟังก์ชันเล็ก)
    ├── Player/       # ระบบผู้เล่น (การเดิน/สเตมินา/อินพุตเฉพาะตัว)
    ├── AI/           # ระบบมอนสเตอร์ / ศัตรู
    ├── Grab/         # ระบบหยิบของและ Physics
    └── Extraction/   # ระบบจุดส่งของและโควตา
```

> **กฎ Include ไม่ให้ `.Build.cs` ชนกัน**: ใน `ScrubbyBooGame.Build.cs` มี `"ScrubbyBooGame"` อยู่ใน `PublicIncludePaths` แล้ว ดังนั้นเวลา `#include` ให้เขียนอ้างอิงจาก Root เสมอ เช่น `#include "Systems/Grab/GrabComponent.h"` **ห้าม** เพิ่มโฟลเดอร์ย่อยใหม่เข้า `PublicIncludePaths` ใน `.Build.cs` โดยไม่จำเป็น

### 2.2 โฟลเดอร์ใน Unreal Project (`Content/ScrubbyBoo/`)
ให้สร้างและเก็บ Asset จริงของเกมทั้งหมดไว้ใต้ `Content/ScrubbyBoo/` แยกออกจากโฟลเดอร์ตัวอย่าง `FirstPerson/` ของ Template:

```text
Content/
└── ScrubbyBoo/
    ├── Blueprints/
    │   ├── Core/            # [ไฟล์กลาง — แจ้งก่อนแก้] BP_GameMode, BP_GameState, BP_PlayerController
    │   ├── Player/          # BP_ScrubbyCharacter และ Component ฝั่งผู้เล่น
    │   ├── AI/              # BP_Monster_*, ST_Monster_*
    │   ├── Valuables/       # BP_Valuable_*
    │   └── Multiplayer/     # Blueprint ฝั่ง Lobby / Session
    ├── DataAssets/          # DA_* (ค่าสเตตัส/ราคา/ความเปราะของไอเทมหรือมอนสเตอร์)
    ├── UI/                  # WBP_* (HUD, Main Menu, Lobby, Scoreboard)
    ├── Maps/                # Lvl_Main, Lvl_Lobby, Lvl_Test_Multiplayer, Lvl_Test_Player
    ├── Art/
    │   ├── Characters/      # SKM_*, SK_*, A_*, AM_*, ABP_*
    │   ├── Valuables/       # SM_*, M_*, MI_*, T_* ของไอเทมมีค่า
    │   ├── Environment/     # SM_*, M_*, MI_*, T_* ของฉากและพร็อพ
    │   ├── Monsters/        # โมเดลและแอนิเมชันของมอนสเตอร์
    │   ├── UI/              # T_*_UI (ไอคอน, โลโก้, รูป UI)
    │   └── VFX/             # NS_* (Niagara Particle Systems)
    └── Audio/               # SFX_*, MS_* (เสียงเอฟเฟกต์, เสียงบรรยากาศ/เพลง)
```

### 2.3 โฟลเดอร์บน Shared Cloud ของ Artist (Google Drive / OneDrive)
สร้างโฟลเดอร์กลางนอก Git แบ่งเป็น 2 ส่วนชัดเจน:

```text
ScrubbyBoo_Art/
├── 01_WIP_Source/           # เก็บไฟล์ต้นฉบับ (.blend, .psd, .spp) — ห้ามเอาเข้า Git
└── 02_Ready_For_Unreal/     # เก็บเฉพาะไฟล์ Export ที่พร้อมเข้า UE (.fbx, .png/.tga, .wav)
    ├── Characters/
    ├── Valuables/
    ├── Environment/
    ├── Monsters/
    ├── UI/
    └── Audio/
```

---

## 3. กฎการตั้งชื่อไฟล์ (Unreal Naming Conventions)

### กฎพื้นฐาน (ต้องทำตามทุกคน)
1. **ใช้ภาษาอังกฤษเท่านั้น** และใช้รูปแบบ `PascalCase` (ตัวแรกของแต่ละคำเป็นตัวพิมพ์ใหญ่ เช่น `SM_Valuable_AntiqueVase`)
2. **ห้ามเว้นวรรค ห้ามภาษาไทย และห้ามใช้อักขระพิเศษ** (ใช้ `_` คั่นระหว่าง Prefix กับชื่อเท่านั้น เช่น `BP_Valuable_Crown` ไม่ใช่ `BP Valuable Crown (1)`)
3. **ห้ามต่อท้ายเวอร์ชันใน `02_Ready_For_Unreal/`**: ห้ามตั้งชื่อแบบ `_v2`, `_final`, `_final2`, `_new`, `_fix` ให้ **เซฟทับชื่อเดิม** เสมอ เพื่อให้ Unreal กด `Reimport` ได้โดย Reference ไม่พัง

### ตาราง Prefix มาตรฐาน

| ประเภทไฟล์ | Prefix | ตัวอย่างชื่อไฟล์ในโปรเจกต์ | หมายเหตุ |
| :--- | :--- | :--- | :--- |
| **Blueprint Class** | `BP_` | `BP_ScrubbyCharacter`, `BP_Valuable_Vase`, `BP_Monster_Stalker` | คลาสลูกจาก C++ หรือ Actor ทั่วไป |
| **UMG Widget (UI)** | `WBP_` | `WBP_HUD`, `WBP_LobbyMenu`, `WBP_QuotaPanel` | หน้าต่าง UI และ Widget ย่อย |
| **DataAsset** | `DA_` | `DA_Valuable_Vase`, `DA_Valuable_GoldBar` | ไฟล์ตั้งค่าข้อมูลของไอเทม/ศัตรู |
| **Level / Map** | `Lvl_` | `Lvl_Main`, `Lvl_Lobby`, `Lvl_Test_Multiplayer` | ด่านเกมและด่านทดสอบ |
| **Static Mesh** | `SM_` | `SM_Valuable_Vase`, `SM_ExtractionCart`, `SM_HospitalDoor` | โมเดล 3D ที่ไม่มีกระดูก |
| **Skeletal Mesh** | `SKM_` | `SKM_PlayerBot`, `SKM_Monster_Stalker` | โมเดล 3D ที่มีกระดูก (Rigged) |
| **Skeleton** | `SK_` | `SK_PlayerBot`, `SK_Monster_Stalker` | โครงกระดูกของ Skeletal Mesh |
| **Animation Sequence** | `A_` | `A_Player_Walk`, `A_Monster_Grab` | คลิปแอนิเมชัน |
| **Animation Montage** | `AM_` | `AM_Player_HitReact`, `AM_Monster_Attack` | Montage สำหรับสั่งเล่นผ่าน Code/BP |
| **Animation Blueprint** | `ABP_` | `ABP_PlayerBot`, `ABP_Monster_Stalker` | ระบบควบคุมแอนิเมชัน |
| **Master Material** | `M_` | `M_ValuableMaster`, `M_PropsMaster` | Material แม่ที่ตั้งค่า Shader หลัก |
| **Material Instance** | `MI_` | `MI_Valuable_Vase`, `MI_Valuable_Crown` | Material ลูกที่ใช้ปรับสี/Texture |
| **Texture** | `T_` | `T_Valuable_Vase_D`, `T_Valuable_Vase_N` | ต้องมี **Suffix** ระบุชนิดท้ายชื่อเสมอ (ดูตารางด้านล่าง) |
| **Niagara System (VFX)** | `NS_` | `NS_GlassShatter`, `NS_ExtractionBeam` | เอฟเฟกต์ Particle |
| **Sound Effect** | `SFX_` | `SFX_ValuableBreak`, `SFX_GrabHum` | เสียงเอฟเฟกต์ (แนะนำ `.wav` 16-bit 44.1/48kHz) |
| **Music / Ambience** | `MS_` | `MS_LobbyTheme`, `MS_HorrorAmbience` | เพลงประกอบหรือเสียงบรรยากาศยาว |

### Suffix สำหรับไฟล์ Texture (`T_<ชื่อ>_<Suffix>`)
- `_D` หรือ `_BC` : Base Color / Albedo / Diffuse (เช่น `T_Valuable_Vase_D.png`)
- `_N` : Normal Map (เช่น `T_Valuable_Vase_N.png`)
- `_ORM` : Packed Map (`R` = Ambient Occlusion, `G` = Roughness, `B` = Metallic) — *แนะนำสำหรับ UE*
- `_R` / `_M` / `_AO` : กรณีแยกไฟล์ Roughness, Metallic, Ambient Occlusion
- `_E` : Emissive Map (ส่วนเรืองแสง)
- `_UI` : รูปภาพที่ใช้ใน UI/HUD โดยเฉพาะ (เช่น `T_Icon_GrabBeam_UI.png`)

---

## 4. ขั้นตอนการส่งและอัปเดตงานของ Artist (Artist Workflow & Unreal Reimport)

### 4.1 มาตรฐานการ Export สำหรับ Unreal Engine
1. **หน่วยและสเกล**: ตั้งหน่วยเป็น **Centimeters (cm)** (`1 Unreal Unit = 1 cm`, ความสูงตัวละครมาตรฐานประมาณ `170–180 cm`)
2. **แกนและ Transform**: ก่อน Export ให้ **Apply Scale / Rotation** ให้เรียบร้อย (`Scale = 1.0`, ด้านหน้าตัวละคร/พร็อพหันทางแกน `+X` หรือสม่ำเสมอทั้งโปรเจกต์)
3. **จุดหมุน (Pivot Point)**:
   - พร็อพวางพื้น / ฉาก: Pivot อยู่ที่ฐานตรงกลางด้านล่าง `(0, 0, 0)`
   - **ของมีค่าที่จะถูกหยิบ (Valuables)**: Pivot ควรอยู่บริเวณจุดศูนย์ถ่วงของวัตถุ เพื่อให้ระบบ Physics Grab ในเกมหมุนและยกได้สมจริง
4. **รูปแบบไฟล์**:
   - โมเดล 3D: `.fbx` (Binary, Smoothing Groups เปิดไว้)
   - Texture: `.png` หรือ `.tga` (ความละเอียดเป็นเลขยกกำลังสอง เช่น `512`, `1024`, `2048`)
   - เสียง: `.wav`

### 4.2 ขั้นตอนเมื่อ Artist แก้ไขไฟล์เดิม (เช่น แก้ UV, แก้โมเดล, แก้สี)
1. Artist **Export ทับไฟล์ชื่อเดิม** ลงในโฟลเดอร์เดิมใน `ScrubbyBoo_Art/02_Ready_For_Unreal/`
2. Artist ส่งข้อความสั้นๆ ในแชททีม เช่น:
   > *"อัปเดต `SM_Valuable_Vase.fbx` แล้ว แก้ Pivot ให้อยู่ตรงกลาง"*
3. **Blueprint Dev (หรือ C++ Dev)** ดาวน์โหลดไฟล์ล่าสุดจาก Cloud มาลงเครื่อง
4. เปิด Unreal Editor → ไปที่ไฟล์เดิมใน Content Browser → **คลิกขวาเลือก `Reimport`** *(หรือ `Reimport With New File` หากเปลี่ยนตำแหน่งไฟล์ต้นทางบนเครื่อง)*
5. Unreal จะอัปเดตไฟล์ `.uasset` เดิมให้ทันที **ทุก Blueprint, Material และ Map ที่ใช้ Asset นี้จะอัปเดตตามอัตโนมัติโดยไม่พัง**
6. กด **Save All** แล้ว Commit & Push `.uasset` ที่อัปเดตขึ้น Git

---

## 5. ขั้นตอนการใช้ Git สำหรับ Blueprint Dev (Git & GitHub Desktop)

### 5.1 การตั้งค่าเครื่องครั้งแรก (ทำครั้งเดียว)
1. ติดตั้ง **Git for Windows** และ **GitHub Desktop**
2. เปิด PowerShell / Terminal แล้วรันคำสั่งนี้ **1 ครั้ง** เพื่อเปิดใช้งาน Git LFS:
   ```powershell
   git lfs install
   ```
   *(สำคัญมาก: ถ้าไม่รันคำสั่งนี้ ก่อน Clone/Pull ไฟล์ `.uasset` จะโหลดมาเป็นไฟล์ข้อความเล็กๆ และเปิดใน Unreal ไม่ได้)*
3. ติดตั้ง **Visual Studio 2022 Community (ฟรี)** โดยเลือก Workload: **"Game development with C++"**
   - **ไม่จำเป็นต้องเปิด Visual Studio หรือเขียนโค้ดเอง**
   - มีไว้เพื่อให้เวลา C++ Dev อัปเดตโค้ดใหม่แล้วคุณ Pull ลงมา Unreal Editor จะสามารถกด Rebuild C++ ให้อัตโนมัติตอนเปิดโปรเจกต์ได้ทันที

### 5.2 ระบบ Branch (ห้ามทำงานตรงบน `main`)
ใช้กฎ **1 ระบบใหญ่ = 1 Branch สั้นๆ (1–3 วัน) = 1 โฟลเดอร์ระบบ** และแตก Branch จาก `main` ล่าสุดเสมอ:
- `main` : **Branch หลักที่เสถียรเสมอ** ต้อง Compile ผ่านและเปิด Editor ได้ตลอดเวลา
- `feat/<ชื่อระบบ>` : Branch สำหรับฟีเจอร์ใหม่ (เช่น `feat/multiplayer-session`, `feat/player-movement`, `feat/enemy-ai`)
- `fix/<ชื่อบั๊ก>` : Branch สำหรับแก้บั๊ก (เช่น `fix/grab-desync`)

**ตัวอย่างลำดับการทำงานจริง (2 คนทำงานพร้อมกันโดยไฟล์ไม่ชน)**:
1. **Multiplayer Dev** แตก `feat/multiplayer-session` (ทำใน `Multiplayer/` + `Blueprints/Multiplayer/`)
2. **Co-worker** แตก `feat/player-movement` พร้อมกัน (ทำใน `Systems/Player/` + `Blueprints/Player/` + `Lvl_Test_Player`)
3. เมื่อทั้งคู่ทำเสร็จและ Merge เข้า `main` แล้ว → ให้ **ลบทิ้ง Branch เก่า** และ **Pull `main` ล่าสุด** ก่อนแตก Branch ใหม่ (เช่น `feat/enemy-ai` ทำใน `Systems/AI/` + `Blueprints/AI/`)

### 5.3 กฎเหล็กป้องกันไฟล์ Binary ชนกัน (`.uasset` / `.umap`)
ไฟล์ `.uasset` และ `.umap` **ไม่สามารถ Auto-Merge แบบไฟล์โค้ดได้** ดังนั้นให้ยึดกฎ 4 ข้อนี้:

1. **แยก Map ทดสอบกัน**:
   - Multiplayer Dev ใช้ `Lvl_Test_Multiplayer` สำหรับเทสต์ระบบเน็ตเวิร์ก
   - Gameplay Dev ใช้ `Lvl_Test_Player` หรือ `Lvl_Main`
2. **จองไฟล์ก่อนแก้ (Call Dibs)**:
   - หากจำเป็นต้องแก้ไฟล์ `.uasset` หรือ `.umap` ที่มีโอกาสใช้ร่วมกัน (เช่น `BP_ScrubbyCharacter`, `WBP_HUD`, `Lvl_Main`) ให้พิมพ์บอกในแชททีมก่อนทุกครั้ง เช่น *"กำลังแก้ `Lvl_Main` อยู่นะครับ"* — **1 ไฟล์แก้ไขได้ทีละ 1 คนเท่านั้น**
3. **ขั้นตอนทำงานประจำวันใน GitHub Desktop**:
   1. กด **Fetch / Pull** `main` ล่าสุดก่อนเริ่มงานเสมอ
   2. สร้าง Branch ใหม่จาก `main` ตั้งชื่อ `feat/<ชื่อระบบ>` (งานสั้นๆ จบใน 1–3 วันแล้ว Merge)
   3. ทำงานใน Unreal Editor / IDE → กด **Save All** (`Ctrl + Shift + S`)
   4. กลับมาที่ GitHub Desktop → ตรวจดูรายการไฟล์ที่เปลี่ยน (ต้องไม่มีขยะจาก `Saved/` หรือ `Intermediate/`) → เขียน Commit Message สั้นๆ ชัดเจน → กด **Commit** → **Push origin**
   5. กด **Create Pull Request** บน GitHub แล้วแจ้ง Git Manager (เมื่อ Merge แล้วให้ Pull `main` และลบ Branch เก่าทิ้ง)
4. **ถ้าเกิด Binary Conflict ขึ้นมาจริงๆ**:
   - **ห้าม**พยายามเปิดไฟล์ `.uasset`/`.umap` มา Merge แบบ Text เด็ดขาด
   - ให้ปรึกษากันแล้ว **เลือกเก็บเวอร์ชันใดเวอร์ชันหนึ่ง** (ของตัวเอง หรือ ของอีกฝั่ง) จากนั้นค่อยเข้าไปแก้งานส่วนที่หายไปซ้ำใน Unreal Editor

### 5.4 กฎป้องกัน Conflict เมื่อทั้ง 2 คนเขียน C++ ด้วย AI
เมื่อใช้ AI ช่วยเขียนโค้ด C++ พร้อมกัน 2 คน ให้ตั้งกฎเหล็ก 5 ข้อนี้เพื่อให้ Merge เข้ารวมกันได้ลื่นไหล:

1. **1 ระบบใหญ่ = 1 โฟลเดอร์ (`Systems/<Feature>/` และ `Blueprints/<Feature>/`)**:
   - ห้ามเขียนระบบใหม่รวมกันในโฟลเดอร์เดียว แต่ให้แยกตาม **ระบบใหญ่** (เช่น `Player/`, `AI/`, `Grab/`, `Extraction/`) ไม่ต้องแตกโฟลเดอร์ย่อยตามฟังก์ชันเล็กๆ ทุกตัว
   - **ห้ามแก้ไฟล์ในโฟลเดอร์ฟีเจอร์ที่เพื่อนกำลังทำอยู่**: ถ้าระบบต้องคุยกัน (เช่น `AI` ตี `Player`) ให้รอ Branch ของเพื่อน Merge เข้า `main` ก่อน หรือสื่อสารผ่าน `UInterface` ใน `Shared/` (หรือ `UGameplayStatics::ApplyDamage`)
2. **เขียนเป็น `UActorComponent` หรือ `Subsystem` แทนการแก้ไฟล์กลาง**:
   - ระบบติดตัวละครให้ทำเป็น `UActorComponent` (เช่น `UPlayerStaminaComponent`) แล้วค่อยเอาไปแปะบน `BP_ScrubbyCharacter` ใน Editor แทนการยัดโค้ดลงไฟล์ C++ `Character` กลาง
   - ระบบจัดการระดับเกม/โลกให้ทำเป็น `UGameInstanceSubsystem` หรือ `UWorldSubsystem` (Unreal สร้างให้อัตโนมัติ ไม่ต้องแก้ไฟล์ `GameInstance` หรือ `GameMode`)
3. **สั่ง AI ให้อ่าน `AGENTS.md` และห้ามแก้ไฟล์นอกโฟลเดอร์ตัวเอง**:
   - ก่อนให้ AI ทำงาน ให้ระบุขอบเขตชัดเจน เช่น *"ทำงานเฉพาะใน `Source/ScrubbyBooGame/Systems/Player/` ห้ามแก้ไฟล์อื่น"*
   - ห้ามปล่อยให้ AI auto-refactor, จัดรูปแบบไฟล์, หรือเปลี่ยนชื่อไฟล์ใน `Core/`, `Multiplayer/` หรือโฟลเดอร์ระบบของเพื่อน
4. **ไฟล์กลางที่ต้องแจ้ง Git Manager ก่อนแก้**:
   - `Source/ScrubbyBooGame/Core/*`, `ScrubbyBooGame.Build.cs`, `ScrubbyBooGame.uproject`, และ `Config/*.ini`
   - ถ้าจำเป็นต้องแก้ ให้แก้เท่าที่จำเป็นแบบเพิ่มต่อท้าย (Additive) แล้วบอกในแชททีมก่อนทุกครั้ง
5. **โค้ด C++ ที่ AI เขียนต้องรองรับ Multiplayer เสมอ**:
   - ต้องทำตามกฎใน `AGENTS.md`: ใช้ `HasAuthority()` คุมฝั่ง Server, Replicate ค่าผ่าน `UPROPERTY(ReplicatedUsing=OnRep_*)`, เรียก `OnRep_*` บน Server ด้วย (Listen-Server Parity), และตั้งชื่อ RPC เป็น `Server_`/`Client_`/`Multicast_` (ห้ามเขียนโค้ดแบบ Single-Player ล้วน)

---

## 6. เช็กลิสต์สำหรับ Git & File Manager (ก่อน/หลัง Merge PR)

1. **ตรวจรายชื่อไฟล์ใน Pull Request ก่อนกด Merge**:
   - ตรวจว่าโค้ด C++ ใหม่อยู่ในโฟลเดอร์ `Systems/<Feature>/` ของงานนั้น และ **ไม่ได้เผลอแก้** `Core/`, `Multiplayer/`, `.Build.cs`, `.uproject`, หรือ `Config/*.ini` โดยไม่ได้แจ้งล่วงหน้า
   - **ต้องไม่มี** `Saved/`, `Intermediate/`, `Binaries/`, `DerivedDataCache/`, `UpgradeLog*.htm`, หรือไฟล์ต้นฉบับอย่าง `.blend`/`.psd`/`.spp` หลุดเข้ามา
   - ตรวจ Prefix/Folder ของไฟล์ใหม่ใน `Content/ScrubbyBoo/` ให้ตรงตามตารางในข้อ 3
   - หากมีโค้ด C++ ที่ AI สร้าง ตรวจเช็กเบื้องต้นว่ามี `HasAuthority()` / Replication ถูกต้อง ไม่พังเมื่อเล่น 2 คนขึ้นไป
2. **หลัง Merge `feat/*` หรือ `fix/*` เข้า `main`**:
   - Pull `main` มาเปิดเทสต์และกด Build (`ScrubbyBooGameEditor Win64 Development`) รวมถึงเปิดเช็กใน Unreal Editor ว่าไม่มี Blueprint Compile Error หรือ Broken Reference
   - หากมีการเพิ่ม/แก้ระบบ C++ หลัก ให้ส่ง **BP Handover Note** (ชื่อ Class, Variables, Events/Delegates) ให้ทีมและแจ้งให้ทุกคน Pull `main` ล่าสุดก่อนแตก Branch ถัดไป
3. **เช็กโควตา Git LFS เป็นระยะ**:
   - ตรวจสอบพื้นที่และ Bandwidth ได้ที่ **GitHub Repo / Account → Settings → Billing** เพื่อกันโควตา LFS เต็มกลางคัน
