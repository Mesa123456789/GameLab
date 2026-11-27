# สถานะงาน AI

อัปเดต 27 พฤศจิกายน 2026 · ส่งต่องาน Player ให้ Claude Code · **IMPLEMENTED - PENDING MANUAL TEST**

| ผู้รับผิดชอบ | งาน | สถานะ |
| --- | --- | --- |
| Codex | Player Foundation, Movement, Grab/Hold และ Player/Object Physics ของเมษา | ต้นแบบผู้เล่นเดียวมีการเดินและกล้องเดิม, Physics Grab สองมือ, Physical Animation, กล้องดีบัก V และ Two Bone IK รอเมษาทดสอบด้วยตนเอง |
| Claude Code | Cleaning Tools, Targets และ Progress | ต้นแบบ Mop, Cobweb Broom, Garbage Bag บน `CleaningSystem` (worktree `GameLab-Cleaning`) **IMPLEMENTED - PENDING MANUAL TEST** Compile ผ่าน ดู [TASKS](TASKS.md#ต้นแบบ-cleaning-tools-mop-cobweb-broom-garbage-bag--27-พฤศจิกายน-2026) |
| โชต | การพัฒนา C++ และระบบในความรับผิดชอบของโชต | คงขอบเขตเจ้าของเดิม |
| เมษา | อนุมัติงานและทดสอบ gameplay ทั้งหมด | อนุมัติต้นแบบ Physics Grab ผู้เล่นเดียวสำหรับรอบ 27 พฤศจิกายน |
| UI | งาน UI ทุกส่วน | พักไว้ |

## ส่งต่องาน Player และ Cleaning

- Player อยู่ที่ `/Game/ScrubbyBoo/Blueprints/Player/` ใช้ `BP_PlayerFoundation`, `BP_PlayerFoundationController`, `BP_PlayerFoundationTestGameMode` และ `ABP_PlayerGrabIK` กับ `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` กดเมาส์ซ้าย/ขวาค้างเพื่อจับ ปล่อยปุ่มเพื่อปล่อยมือ และกด `V` ดู Manny ทั้งตัว
- ค้างทดสอบมือสัมผัสพร็อพเมื่ออยู่ในระยะแขนและตรวจการคืนฟิสิกส์เมื่อปล่อย `HoldDistance` เดิม 170 cm อาจเกินระยะแขน มุมบุคคลที่หนึ่งซ่อน Manny ทั้งตัวเพื่อกันบังกล้อง งานเชื่อม Cleaning และ Interact ของ Player จริงยังค้าง
- Cleaning อยู่ที่ `/Game/ScrubbyBoo/Blueprints/Cleaning/` กับ `/Game/ScrubbyBoo/Maps/Lvl_Test_Cleaning` ใน worktree `C:/Users/masap/GameLab-Cleaning` ใช้ fixture แยกจาก Player จริง ดู [แนวทางเชื่อมต่อ](INTERFACES.md#ส่งต่อการเชื่อมแบบผู้เล่นเดียว)
- `PlayerSystem` อยู่ที่ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ไฟล์ต้นแบบยังอยู่ใน working tree ของ `C:/Users/masap/GameLab` และยังไม่มี review commit การเปิด `CleaningSystem` อย่างเดียวจึงไม่เห็นไฟล์ Player ชุดนี้ ดูผล Compile และงานค้างใน [TASKS](TASKS.md#ส่งต่อ-player-prototype)
- ใช้ [Implementation Only](../../AGENTS.md#workflow--implementation-only) กับงานนี้และงานถัดไป รอเมษายืนยันผลทดสอบก่อนเปลี่ยนสถานะ gameplay

## Unreal MCP ที่ตรวจแล้ว

- **Cleaning (Claude Code):** Editor แยกเปิดโปรเจกต์ `C:/Users/masap/GameLab-Cleaning/ScrubbyBooGame.uproject` ด้วยแฟล็ก EngineWire เดียวกันและ env `MCP_NATIVE_PORT=3001` Claude Code ตั้ง `unreal-engine` แบบ stdio local scope ให้ `UE_PROJECT_PATH=C:/Users/masap/GameLab-Cleaning` และ `UNREAL_MCP_URL=http://127.0.0.1:3001/mcp` ตรวจแล้วว่า `get_project_settings` ตอบ `projectDir` ของ worktree worktree ยังไม่มี Binaries ของตัวเองเพราะ UnrealBuildTool ปฏิเสธการ build ขณะ Live Coding ของ Editor อีกตัวทำงานอยู่ จึงคัดลอก `Binaries/Win64` ที่ build จาก commit เดียวกัน (`2a8f550`) มาใช้ ไฟล์นี้ถูก gitignore

- Unreal Editor 5.6.1 ที่ `C:/Program Files/Epic Games/UE_5.6/Engine/Binaries/Win64/UnrealEditor.exe` ใช้ EngineWire plugin **0.6.0** และ Node package `unreal-engine-mcp-server` **0.6.0** ปลั๊กอินอยู่นอก repository ที่ `%LOCALAPPDATA%/EngineWire/0.6.0-UE5.6/installed/EngineWire/`
- เปิดจาก `C:/Users/masap/Desktop/ScrubbyBooGame - EngineWire.lnk` ตัวเปิดส่ง path ของ `ScrubbyBooGame.uproject` และ `-PLUGIN="<path to EngineWire.uplugin>"` พร้อม `-EnablePlugins=EngineWire,MovieRenderPipeline,MoviePipelineMaskRenderPass` รอบแรก EngineWire โหลดไม่สำเร็จเพราะหา DLL ของ Movie Render Pipeline และ Mask Render Pass ไม่พบตอนโหลดโมดูล การเปิดสองปลั๊กอินที่มากับ Engine ผ่านแฟล็กนี้แก้ได้โดยไม่แก้ `.uproject`
- Codex ลงทะเบียน `unreal-engine` แบบ stdio ใน `C:/Users/masap/.codex/config.toml` ใช้ Node รัน `%LOCALAPPDATA%/EngineWire/0.6.0-UE5.6/client/node_modules/unreal-engine-mcp-server/dist/cli.js` พร้อม `UE_PROJECT_PATH=C:/Users/masap/GameLab` และ `UNREAL_MCP_URL=http://127.0.0.1:3000/mcp` ตัว proxy อ่าน capability token จาก `Saved/MCP/` เก็บค่านั้นในเครื่องเท่านั้น
- เมื่อเริ่มงานที่ต้องแก้ asset ให้เรียก `manage_level.list_levels` ตรวจชื่อโลก และใช้ `search` / `describe` สำหรับ capability ที่จำเป็น หาก MCP ไม่พร้อม ให้รายงาน blocker ตาม Implementation Only
- การตรวจแบบอ่านอย่างเดียวผ่าน Codex สำเร็จทั้งชื่อโปรเจกต์และเวอร์ชัน UE, แผนที่, Actor, Blueprint, สถานะ Editor, ค้นหา capability และภาพ viewport คำขอ HTTP ที่ไม่มี token ได้ 401 การตั้งค่านี้เป็นของเครื่องเมษา

## การหยุดแผน C++ เดิม

- ก่อนรับคำสั่งเปลี่ยนแนวทาง Codex สร้าง `Source/ScrubbyBooGame/Systems/Player/ScrubbyPlayerCharacter.h` และ `ScrubbyPlayerCharacter.cpp` บน `PlayerSystem` ทั้งสองไฟล์ยัง untracked และได้รับการเก็บไว้ตามคำสั่งเมษา P-01 ใหม่อ้างคลาส `Character` ของ Unreal โดยตรงและไม่มี dependency กับสองไฟล์นี้
- ก่อนเปลี่ยนแนวทาง UnrealBuildTool สร้างไฟล์ Visual Studio สำเร็จ คำสั่ง compile Editor target ของ C++ เดิมยังไม่ได้รัน ไม่มีผล Compile Blueprint หรือ PIE ของ P-01
- การอนุมัติแผน C++ เดิมสิ้นสุดแล้ว เมษาอนุมัติ P-01 แบบ Blueprint Single Player และการจัดการไฟล์ C++ เดิมยังต้องขออนุมัติแยก

## Git และโครงสร้างปัจจุบัน

- รอบปรับ workflow ตรวจพบ local branches `main` และ `PlayerSystem` โดย checkout อยู่ที่ `PlayerSystem` commit `2a8f550` มี working directory เดียวและยังไม่มี `CleaningSystem` งาน Player ทุกประเภทใช้ `PlayerSystem` เดิม ทั้งสอง AI ผลัดกันทำงานเมื่อใช้ working directory เดียวกัน
- โครงสร้าง gameplay ปัจจุบันอยู่ใต้ module เดียว `Source/ScrubbyBooGame/` มี `Core/`, `Multiplayer/`, `Shared/`, `Systems/{Player,Grab,AI,Extraction}/` และ template variants ใต้ `Content/ScrubbyBoo/` ใช้ `Blueprints/`, `Maps/`, `Art/`, `Audio/`, `DataAssets/` และ `UI/` ยังไม่มีโฟลเดอร์เฉพาะ Cleaning
- เอกสาร AI อยู่ที่ root และ `Docs/AI/` ส่วน `Docs/Docs/` เป็นโครงสร้างซ้อนที่พบใน working tree ก่อนรอบนี้ การจัดตำแหน่งเอกสารอ้างอิงต้องขออนุมัติก่อน

## ผลสำรวจ

- `PlayerSystem` อยู่ที่ `2a8f550` ตรงกับ `main` และ ref `origin/main` ที่มีอยู่ในเครื่อง ไม่มี staged changes หรือการเปลี่ยนแปลงใน `Source/`, `Config/` และ `Content/` ตอนตรวจครั้งแรก ก่อนแก้เอกสารพบ `Docs/MultiplayerArchitecture.md` และ `Docs/TeamWorkflow.md` แสดงเป็น deleted ส่วนไฟล์ชื่อเดียวกันใต้ `Docs/Docs/` ยัง untracked และตรงกับ blob ใน `HEAD` ทั้งสองไฟล์ ยังพบ Technical Architecture และเอกสาร AI ฉบับเก่าเป็น untracked ไม่มีหลักฐานว่าใครเป็นผู้ย้ายไฟล์
- โปรเจกต์มี runtime module เดียวชื่อ `ScrubbyBooGame` พร้อม Game และ Editor targets ใช้ Enhanced Input และมี StateTree, GameplayStateTree และ ModelingToolsEditorMode ใน project descriptor ส่วน NetCore และ OnlineSubsystem ปรากฏเป็นแผนในเอกสารหรือคอมเมนต์ ยังไม่ได้ประกาศเป็น dependency ของ module นี้
- `Core/` มี Character, PlayerController, GameMode และ CameraManager จาก First Person template ส่วน `Variant_Horror/` และ `Variant_Shooter/` เป็นตัวอย่างอ้างอิง `Systems/Player/`, `Grab/`, `AI/`, `Extraction/`, `Shared/` และ `Multiplayer/` มีเพียง scaffold ยังไม่มีโฟลเดอร์ `Systems/Cleaning/` หรือ asset เกมจริงใต้ `Content/ScrubbyBoo/` ใน checkout นี้
- `DefaultEngine.ini` ตั้ง `Lvl_FirstPerson` และ `BP_FirstPersonGameMode` เป็นค่าเริ่มต้น ส่วน `DefaultEditor.ini` มี SimpleMap ที่อ้าง path ตัวอย่างเดิมซึ่งไม่พบใน Content ต้องตั้ง pawn ทดสอบให้ถูกใน Editor เพื่อให้ผลทดสอบใช้ตัวละครใหม่ ตรวจแล้วว่ามี `IMC_Default`, `IMC_MouseLook`, `IA_Move`, `IA_Look`, `IA_MouseLook` และ `IA_Jump` จาก template
- `.uproject` ระบุ EngineAssociation `5.6` และ `Engine/Build/Build.version` ของ UE_5.6 บนเครื่องยืนยัน patch 5.6.1 มีไฟล์ Git LFS ของ Manny, Quinn, `SK_Mannequin`, `PA_Mannequin` และ `ABP_Unarmed` ใช้ Manny เป็นโมเดลชั่วคราว ยังไม่ได้ตรวจการโหลดและความเข้ากันของ mesh, skeleton, physics asset และ AnimBP ใน Editor
- อ่าน WorkPlan V1 ครบ 11 หน้า พบไฟล์ในเครื่องที่ `C:/Users/masap/Downloads/Scrubby Boo!_WorkPlan Version 1.pdf` เอกสารยังไม่มีสำเนาใน checkout จึงต้องให้ผู้ทำงานต่างเครื่องเข้าถึงฉบับเดียวกันก่อนอ้าง requirement อ่าน Technical Architecture v0.1 และเอกสาร Markdown ทั้งหมดใน `Docs/` แล้ว เอกสาร architecture และ team workflow อยู่ใต้ `Docs/Docs/` ส่วน root `CLAUDE.md` กับ `INTERFACES.md` ถูกเพิ่มในรอบวางแผนก่อนหน้า

## ความขัดแย้งและเรื่องที่ต้องยืนยัน

| หลักฐาน | แนวทางรอบนี้ |
| --- | --- |
| WorkPlan หน้า 8 ระบุ Ghost AI ให้โชต ส่วนบันทึกหน้า 9–10 ใส่ชื่อเมษาใน AI State Machine | ยึดขอบเขตล่าสุดจากเมษา ให้ Ghost AI, Voice Interaction และ Multiplayer infrastructure เป็นระบบคุ้มครอง ส่งประเด็น ownership ให้ทีมยืนยันเมื่อต้องเชื่อมงาน |
| WorkPlan หน้า 8 และ 11 มี UI ในแผนของเมษา | พัก UI ตามคำสั่งล่าสุด ให้ Cleaning ส่งข้อมูลสถานะสำหรับการเชื่อมในอนาคต |
| AGENTS ฉบับเก่าระบุผู้ใช้เป็น Multiplayer Core Dev/Git Manager และใช้ quota, extraction และ valuables เป็นตัวอย่างแกนเกม เอกสาร AI เดิมให้ Claude วางแผน/รีวิวเป็นค่าเริ่มต้น | เมษาเป็น Gameplay Developer, Codex ทำ Player/Physics และ Claude Code ทำ Cleaning ยึด 100% Cleaning แล้วไปจุดออกตาม WorkPlan หน้า 2 และ 9 ให้ Multiplayer Architecture กำกับวิธีทำ replication |
| Technical Architecture เสนอ phases, Physics Handle และ state ของ body physics ส่วน WorkPlan มีทั้งข้อสรุปและค่าทดลอง | ใช้เป็นตัวเลือกทางเทคนิค ค่าแรงชน ระยะถือ การยกสองคน เวลาลุก การคืนอุปกรณ์ที่เอื้อมไม่ถึง การนับเศษเพิ่ม และ death/revival ยังต้องยืนยันก่อนทำส่วนที่เกี่ยวข้อง |
| Team Workflow §5.2–5.3 ใช้ `feat/` และ `fix/` จาก `main` พร้อมขั้นตอน push/merge/ลบ branch | งานของเมษาใช้ branch ตามระบบและขออนุมัติตาม [AGENTS.md](../../AGENTS.md) ส่วนโครงสร้าง การแบ่งไฟล์ และการตั้งชื่อให้ยึด Team Workflow |
| Team Workflow จัดระบบหยิบและ Object Physics ไว้ใน `Systems/Grab/` และกัน `Shared/` ไว้สำหรับสัญญากลาง | ยืนยันเจ้าของและไฟล์จริงของ Hold/Object Physics ก่อนทำ P-03 ใช้โฟลเดอร์เดิมตามหน้าที่และสิทธิ์ ห้ามทำระบบหยิบซ้ำใน `Systems/Player/` การแก้ `Shared/` ต้องได้รับอนุญาต |

## สถานะ architecture review

- ผลตรวจอิสระจากซอร์สและ WorkPlan หน้า 1–2 และ 9 ปรากฏใน [แผน P-01](TASKS.md) และ [สัญญาร่วม](INTERFACES.md) ข้อเสนอ P-01 ใช้ `ACharacter` กับกล้องอ้างอิง pawn แยกจาก socket `head` ของ template และเลือกตัวละครผ่าน GameMode Override ในแผนที่ทดสอบ เพราะค่าโปรเจกต์เดิมชี้ไปที่ First Person template
- 27 พฤศจิกายน 2026 11:53 น. Claude Code รีวิวแผน P-01 และสัญญาร่วมฉบับที่ Codex อัปเดตเวลา 11:50–11:51 น. แล้ว ข้อค้นพบ R-01 ถึง R-12 อยู่ใน [TASKS](TASKS.md#ผลรีวิวสถาปัตยกรรมโดย-claude-code--รอเมษาตัดสิน) สรุปว่าแนวทาง P-01 ใช้ได้ (`ACharacter` และกล้องยึด capsule ช่วยแก้ปัญหากล้องขึ้นกับ socket `head` ที่ ragdoll จะพาเหวี่ยง) หากเมษาเลือกชื่อคลาส/Blueprint ถาวรก่อน ส่วน P-02 ขึ้นไปยังไม่พร้อม เพราะสัญญาร่วมต้องใช้ `Shared/` ยังไม่มีการใช้งานแบบต่อเนื่อง (BeginUse/EndUse) และยังไม่ได้เลือก network model ของการถือ ยังไม่ได้ build หรือทดสอบใน Editor
- ข้อคิดเห็น R-01 ถึง R-12 เป็นรีวิวแผนเดิม เก็บไว้เพื่ออ้างอิง ข้อเสนอใหม่ของ PC-01 ใช้ Blueprint Interface ที่เสนอในโฟลเดอร์ Content เดิม แผน Blueprint P-01 ไม่ต้องรอสัญญานี้

## จุดติดขัดและการตรวจสอบที่ยังค้าง

- P-01 ได้รับอนุมัติชุด Blueprint และ test map ตาม [TASKS](TASKS.md) ส่วน PC-01 ต้องยืนยันผู้ดูแล path, pins และ authority ก่อนสร้าง BPI ให้เมษาหรือทีมจัดเตรียม `CleaningSystem` ตาม workflow
- Unreal MCP ใช้งานกับ Editor ได้จริงและใช้สร้าง แก้ ตรวจกราฟ และ Compile Blueprint ของ P-01 แล้ว
- บันทึก P-01 ใน [TASKS](TASKS.md) เป็นประวัติจากก่อนเปลี่ยน workflow ส่วนผล gameplay ของต้นแบบ Physics Grab ปัจจุบันรอเมษายืนยัน
- ใช้ [TASKS](TASKS.md) สำหรับลำดับงาน และ [INTERFACES](INTERFACES.md) สำหรับจุดเชื่อม Player–Cleaning
