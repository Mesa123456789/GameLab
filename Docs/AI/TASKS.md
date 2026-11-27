# Backlog · Player, Physics และ Cleaning

สถานะเริ่มต้นของทุก task คือ **รอเมษาอนุมัติ** ตรวจ branch และ Git status ก่อนเริ่ม กำหนด base commit และไฟล์จริงตาม task

- P-01 ถึง P-06 ใช้ `PlayerSystem` เดิมและทำด้วย Blueprint เท่านั้น Cleaning พัฒนาแยกบน `CleaningSystem` เมื่อเมษาหรือทีมจัดเตรียม ห้าม AI สร้าง branch หรือ worktree
- PC-01 และ QA-01 ระบุ asset revision ที่ใช้ร่วมกัน ให้แต่ละระบบใช้ fixture ของตนเพื่อทดสอบระหว่างที่อีกระบบยังไม่พร้อม เมื่อใช้ working directory เดียวกันให้ผลัดกันทำงาน
- ตรวจโฟลเดอร์ คลาส โมดูล interface และ naming ที่มีอยู่ก่อนระบุไฟล์ใหม่ หากต้องใช้โฟลเดอร์ใหม่ ให้เสนอ path, จุดประสงค์, เหตุผลที่ใช้ของเดิมไม่ได้ และรายการไฟล์ที่คาดไว้ แล้วรอเมษาอนุมัติ

- งานพัฒนาใช้ [Implementation Only](../../AGENTS.md#workflow--implementation-only) เมษาเป็นผู้ทดสอบ gameplay ตามเกณฑ์ที่อนุมัติ รายการใน backlog ระบุเกณฑ์ของแต่ละ task

## การจับและทำความสะอาดแบบกด F

**IMPLEMENTED - PENDING MANUAL TEST** · 27 พฤศจิกายน 2026 · ผู้เล่นเดียว

- เมษาอนุมัติให้แทนที่ Physics Grab และการเพิ่ม progress ตามความเร็วใน worktree `C:/Users/masap/GameLab-Cleaning` บน branch `CleaningSystem` ฐาน `6a4f7e049cded26412795eb940b88a1afd17f5a9` งานนี้ยังไม่มี review commit
- แก้ `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation`, `/Game/ScrubbyBoo/Blueprints/Cleaning/BP_CleaningTool` และ `/Game/ScrubbyBoo/Blueprints/Cleaning/BP_CobwebBroom` ผ่าน Unreal MCP ผล Compile และ Save ผ่านในการแก้ gameplay ตัวอ่าน IK เดิมใช้ตำแหน่ง Hand Collider และ Grip ของมือรอง
- มือซ้ายและขวาใช้ Sphere Collider แยกกัน เปิด query overlap ระหว่างเหยียดมือว่าง ใช้รัศมีทดลอง 40 cm จุดเหยียดอยู่หน้ากล้อง 90 cm ต่ำลง 32 cm และแยกซ้ายขวาข้างละ 16 cm อุปกรณ์แนบกับมือหลักโดยอิง `GripPointA` ไม้กวาดตั้ง Pitch ของ Pivot 90° เพื่อให้หัวชี้ไปข้างหน้า
- เมื่อถือ ปิด Simulate Physics ที่ราก กัน Pawn ที่รากและปิด collision ของ mesh ลูกที่เป็นภาพ หัวทำความสะอาดใช้ `HeadCleanZone` ต่อ มือสุดท้ายคืนการจำลองและค่า collision ที่บันทึกไว้ การโอนมือหลักย้าย attachment ไปมือที่ยังจับอยู่
- หัวม็อบสัมผัส `BP_Stain` หรือหัวไม้กวาดสัมผัส `BP_Cobweb` ขณะถือเครื่องมือและกด F จึงเดิน timer ทุก 0.1 วินาที การถูสลับตำแหน่งมือ ±3 cm ตามแกนข้าง ค่าทดลอง `CleaningDuration` 4 วินาที ปรับได้ใน Blueprint Details
- `BP_Stain` และ `BP_Cobweb` เก็บ `CleaningProgress` แบบ Float 0–100 เริ่มที่ 0 และสร้าง Dynamic Material Instance ครั้งเดียว ใช้ `Opacity = 1 - CleaningProgress / 100` เมื่อครบ 100 ซ่อน dirt และปิด collision การปล่อย F การขาด contact และการปล่อยมือสุดท้ายหยุดถูโดยเก็บ progress เดิม ถุงขยะใช้ `CollectGarbage` ผ่าน E ตามเดิม
- Test level `/Game/ScrubbyBoo/Maps/Lvl_Test_Cleaning` · WASD เดิน · เมาส์มอง · เมาส์ซ้าย/ขวาค้างเพื่อเหยียดและจับ ปล่อยปุ่มเพื่อวาง · F ค้างเพื่อถู · E เก็บขยะเมื่อถือถุง · V ดูตัวละครจากกล้องดีบัก

- งานเก็บส่วนที่เลิกใช้ค้างเมื่อ Unreal MCP แจ้ง `Native MCP session limit reached` กราฟ input และ Tick ของ Physics Grab เดิมถูกถอดแล้ว ฟังก์ชันเดิมกับ `GrabHandleA` และ `GrabHandleB` ยังอยู่ใน asset และไม่มีเส้นทางจาก input ใหม่ กราฟวัดความเร็วหัวเครื่องมือถูกถอดแล้ว
- ผลข้างต้นเป็น Compile และ Save รอผล gameplay จากเมษา ตัวเชื่อม MCP ค้างก่อนเปิดแท็บ Blueprint สำหรับส่งมอบ

- แก้กราฟหลังใช้ Blueprint Anti-Pasta โหมด Layout (Route) บน `CleaningSystem` ฐาน `6a4f7e0` คืนการเชื่อม 28 จุดใน `BP_CleaningTool` EventGraph ผ่าน Unreal MCP รวมเส้น HeadCleanZone, timer และ progress ตรวจ 107 การเชื่อมกับกราฟก่อนจัดโหนดแล้วตรงกัน ส่วน 69 การเชื่อมที่ตรวจใน `BP_PlayerFoundation` EventGraph ตรงกันอยู่แล้ว Compile และ Save `BP_CleaningTool` ผ่าน เปิด asset ใน `Lvl_Test_Cleaning` แล้ว รอเมษาทดสอบ gameplay

### Checklist ทดสอบการจับและทำความสะอาด

1. ใช้ WASD และเมาส์ ตรวจมุมมองบุคคลที่หนึ่งและกด V ดู Manny ทั้งตัวกับการเหยียดแขนทีละข้าง
2. เข้าหาวัตถุด้วยมือซ้ายและขวาทีละข้างโดยให้ Hand Collider สัมผัส ลองถือคนละชิ้นและจับชิ้นเดียวสองมือ ปล่อยมือหลักแล้วตรวจว่าอีกมือยังถือ ปล่อยมือสุดท้ายแล้วตรวจฟิสิกส์และ collision
3. ใช้ม็อบกับคราบพื้นและไม้กวาดกับหยากไย่ กด F ค้าง ตรวจท่าถูและ opacity ที่ลดลง ทดสอบเครื่องมือผิดชนิด ด้ามแตะ dirt และสัมผัสโดยยังไม่กด F
4. ปล่อย F แยกหัวออก และปล่อยเครื่องมือทีละกรณี ตรวจว่าหยุดทันทีและกลับมาถูต่อจากค่าเดิม เมื่อครบ 100 ตรวจว่า dirt หายและ collision ปิด
5. ถือถุงขยะแล้วกด E ที่ขยะใกล้ ๆ ตรวจว่าถุงเก็บขยะผ่านการกด E และการสัมผัสอย่างเดียวไม่เก็บ

รายละเอียดต้นแบบด้านล่างเป็นประวัติ ใช้กติกาในหัวข้อนี้สำหรับการทดสอบบน `CleaningSystem`

## ส่งต่อ Player Prototype

**IMPLEMENTED - PENDING MANUAL TEST** · Single Player · `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` อยู่ใน working tree ยังไม่มี review commit

| Asset Path | หน้าที่ที่ทำแล้ว |
| --- | --- |
| `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation` | WASD, mouse look, กล้องเดิม, Physics Handle สองมือ, จุดจับแยกต่อมือ, Physical Animation และกล้องดีบัก `V` |
| `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundationController` | ลงทะเบียน Enhanced Input สำหรับการเดินและมอง |
| `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundationTestGameMode` | เลือก Player pawn และ Controller ของแผนที่ทดสอบ |
| `/Game/ScrubbyBoo/Blueprints/Player/ABP_PlayerGrabIK` | Two Bone IK บน Manny ตัวหลักจากจุดจับเดิม คืนฟิสิกส์เฉพาะแขนเมื่อปล่อย |
| `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` | แผนที่ทดสอบ Player และพร็อพฟิสิกส์ |

- ค่าที่ปรับได้อยู่ใน `Class Defaults → ScrubbyBoo → Grab` (`GrabDistance`, `HoldDistance`) และ `Physical Animation` (`SpineStrength`, `ArmStrength`, `NeckStrength`, `HeadBackwardTilt`, `HeadAngularLimit`, `HeadDamping`, `MaximumForwardSpineBend` และค่าประกอบเดิม)
- ค้างยืนยันการเอื้อม การจับชิ้นเดียวกันคนละจุด การคืนฟิสิกส์ และท่าทางตอนเดิน/หมุน `HoldDistance` 170 cm อาจเกินระยะแขนเพราะ IK ไม่ยืดกระดูก มุมบุคคลที่หนึ่งยังซ่อน Manny ทั้งตัว การแสดงแขนมุมบุคคลที่หนึ่งยังค้าง
- Cleaning ต้นแบบอยู่ใน worktree `GameLab-Cleaning` ใช้ `/Game/ScrubbyBoo/Blueprints/Cleaning/` และ `/Game/ScrubbyBoo/Maps/Lvl_Test_Cleaning` งานร่วมที่ค้างคือ holder notification และ Interact สำหรับถุงขยะ ตาม [INTERFACES](INTERFACES.md#ส่งต่อการเชื่อมแบบผู้เล่นเดียว)

## ขอบเขตไฟล์

| ชุดไฟล์ | สิทธิ์หลังอนุมัติ |
| --- | --- |
| Player | Codex ใช้ Blueprint ใน `Content/ScrubbyBoo/Blueprints/Player/` และชุด map ทดสอบที่เมษาอนุมัติ ใช้คลาสและ API เดิมได้ตามสิทธิ์ |
| Cleaning | Claude Code ใช้ Blueprint และ asset ที่เมษาอนุมัติ ให้ตรวจโครงสร้างเดิมก่อนเสนอ path |
| สัญญาร่วม | ใช้ [INTERFACES.md](INTERFACES.md) ระบุผู้ดูแล path, inputs, outputs และ authority ก่อนสร้าง Blueprint Interface asset |
| ส่งมอบให้ทดสอบ | บันทึกผล Compile Blueprint และ checklist ใน `Docs/AI/` เมษารันการทดสอบ gameplay และยืนยันผล |

## ลำดับงาน

| ID | ผู้รับผิดชอบ | งานและที่มา | Dependency | ไฟล์ | เกณฑ์รับงาน |
| --- | --- | --- | --- | --- | --- |
| PC-01 | Codex + Claude Code ร่างร่วมกัน ผู้เขียนหนึ่งคนตามที่เมษากำหนด | ยืนยันสัญญา Player–Cleaning และวิธีประกอบ test pawn | เมษาอนุมัติ draft, เจ้าของ/ผู้เขียน, asset path, pins และ authority | สัญญาร่วม | ระบุผู้เรียกและผู้รับ, payload, server validation, lifecycle, replication condition, วิธีทดสอบแยกและ integration พร้อมบันทึกจุดที่ยังต้องตัดสินใจ |
| P-01 | Codex | Single Player Blueprint: WASD, mouse look, กล้องบุคคลที่หนึ่ง, collision, Enhanced Input, default spawn และแผนที่ทดสอบ | เมษาอนุมัติแล้ว | Blueprint สามไฟล์และ Lvl_Test_Player | ผ่านการตรวจ spawn, WASD, mouse look, กล้อง, collision และ Compile ใน Standalone PIE หนึ่งผู้เล่น |
| P-02 | Codex | Interaction: หาเป้าหมายและส่งเจตนาให้ server ตรวจ (หน้า 9) | P-01 และ PC-01 สำหรับจุดเชื่อม | Player | Host กับ Client ใช้ fixture ผ่านเส้นทางเดียวกัน เป้าหมายผิด/นอกระยะ/ถูกบังตามกติกาที่อนุมัติไม่มีผล Client แก้ state เป้าหมายเองไม่ได้ |
| P-03 | Codex | หยิบ ถือ วางและปล่อยวัตถุ น้ำหนักมีผลกับการเคลื่อนที่ตามขอบเขตที่อนุมัติ (หน้า 9) | P-02 และอนุมัติกติกา hold/weight | Player | Server เป็นผู้แก้ held state ทดสอบวางและปล่อยด้วย test props ต่างมวล ผู้เล่นสองคนขอถือเป้าหมายเดียวกันแล้วผลสอดคล้องกัน ไม่มีวัตถุติดค้างเมื่อ actor หายหรือเลิก possession ผู้เข้าใหม่เห็น held state ปัจจุบัน |
| P-04 | Codex | ต้นแบบ reaction เมื่อเอื้อม ถือ ถูกดึง ชน เซและล้ม โดยคงการเดิน/เล็งที่แม่น (หน้า 9) | P-01, P-03 และอนุมัติเงื่อนไข impact/recovery | Player | ใช้ interaction/collision fixture ให้ server ตัดสิน transition และการปล่อยของเมื่อเข้าเงื่อนไข ทดสอบ host/client และ network emulation อ้าง socket/physics asset ผ่านข้อมูลตั้งค่า |
| P-05 | Codex | ยื่นให้เพื่อนกดรับ ดึงแขน และลากผู้เล่นที่ล้ม โดยผู้ถูกดึงต้านหรือออกได้ (หน้า 9) | P-03, P-04 และอนุมัติกติกาการโอน/ดึง/ลาก | Player | ทดสอบสองผู้เล่นทั้งสองทิศทาง Server ยืนยันการรับและกันผลซ้ำ เมื่อหลุด เป้าหมายหาย หรือ disconnect การถือ/ดึงจบสอดคล้องกัน ใช้เพื่อนที่ล้มใน fixture |
| P-06 | Codex + เมษาใน Editor | สลับ Manny เป็นโมเดลจริง | P-01 และ rig/mesh/animation พร้อมส่งมอบ ทดสอบ P-02 ถึง P-05 ที่ทำแล้วซ้ำ | Player | ตั้ง mesh/skeleton, AnimBP, Physics Asset, IK, socket, offsets และ collision ตาม rig ใหม่ แล้วทดสอบกล้อง การเคลื่อนที่ interaction และ physics ที่รองรับใน Solo/PIE |
| C-01 | Claude Code | สถานะของงานบังคับและความคืบหน้า 0–100% (หน้า 2 และ 9) | เริ่มด้วย fixture ฝั่ง Cleaning ได้หลังอนุมัติ นิยามงานบังคับและวิธีนับต้องได้รับการยืนยัน | Cleaning | Server คุมงานและผลรวม คำขอซ้ำไม่เพิ่ม progress ซ้ำ เมื่อทุกงานบังคับเสร็จได้ 100% และเผยสถานะพร้อมออกให้ผู้เรียกอ่าน ทดสอบ late join ด้วยข้อมูลปัจจุบัน |
| C-02 | Claude Code | ถุงขยะเก็บ/ขนไปจุดทิ้ง ไม้กวาดกวาดหยากไย่ ม็อบถูคราบพื้นและเก็บ progress เมื่อพัก (หน้า 9) | C-01, PC-01 และยืนยันวิธีใช้/ค่าที่ยังเปิด ใช้ fixture ได้ก่อน P-02/P-03 เสร็จ | Cleaning | ทดสอบถุงขยะที่จุดทิ้ง หยากไย่มุม/ผนัง/เพดาน และพัก/ถูต่อที่คราบเดิม ตรวจว่าเครื่องมือผิดหรือเป้าหมายไม่พร้อมไม่มีผล และผู้เล่นสองคนทำงานเดียวกันแล้วผลตรงกัน |
| C-03 | Claude Code | คืนคราบเดิมเป็นงานสกปรกเมื่อมีเหตุจากระบบภายนอก (หน้า 9) | C-01, C-02 และยืนยันข้อมูล integration ก่อนเชื่อมระบบภายนอก | Cleaning | เรียกผ่าน fixture ฝั่ง server แล้วเปิดงานเดิมและอัปเดต progress หนึ่งครั้ง จำลองการเรียกซ้ำและ late join โดยไม่ต้องใช้ Ghost AI |
| QA-01 | Codex + Claude Code ในไฟล์ของตน และเมษาช่วยตั้ง Editor | ทดสอบ Player ใช้อุปกรณ์ Cleaning ผ่านสัญญาร่วม | PC-01, P-02, P-03, C-01, C-02 และ test pawn ที่ประกอบแล้ว | ทดสอบร่วม | Solo และ PIE Listen Server ≥2 ผู้เล่น สลับ host/client เป็นผู้เริ่มการกระทำ ทดสอบ invalid request, อุปกรณ์หลุดหรือถูกทำลาย, งานเป้าหมายหาย, การทำงานพร้อมกัน, late join และ latency/loss พร้อมบันทึกผลจริง |

## P-01 · Single Player Blueprint

ขอบเขตล่าสุดแทนที่แผน P-01 เดิม ทำบน `PlayerSystem` ด้วย Unreal MCP อ้าง WorkPlan หน้า 1–2 และ 9 สำหรับมุมมองบุคคลที่หนึ่งและการเดิน/เล็ง งาน multiplayer ทั้งหมดรออนุมัติแยก

| Asset | สิ่งที่ใช้งาน |
| --- | --- |
| `Content/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation.uasset` | `Character` และ Capsule เดิม กล้องติด Capsule ที่ Z=64 cm ใช้ Manny กับ `ABP_Unarmed` จาก asset เดิม กราฟ `IA_Move` ต่อ `Add Movement Input` ตาม yaw และ `IA_MouseLook` ต่อ yaw/pitch โดยตรง |
| `Content/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundationController.uasset` | ลงทะเบียน `IMC_Default` และ `IMC_MouseLook` จาก template ใน Enhanced Input Local Player Subsystem ตอนเริ่มเล่น |
| `Content/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundationTestGameMode.uasset` | ใช้ `BP_PlayerFoundation` เป็น Default Pawn Class และ `BP_PlayerFoundationController` เป็น Player Controller Class |
| `Content/ScrubbyBoo/Maps/Lvl_Test_Player.umap` | ใช้ GameMode Override เฉพาะแผนที่ มี PlayerStart, พื้น, แสง และผนังสำหรับตรวจ collision |

### ประวัติการทดสอบ P-01 ก่อนเปลี่ยน workflow

Standalone PIE หนึ่งผู้เล่นเท่านั้น ใช้ Unreal Editor 5.6.1 ผ่าน MCP ใช้ input เมาส์ใน viewport และอ่านค่ากล้องกลับผ่าน MCP หยุด PIE หลังทดสอบเสร็จ

| เกณฑ์ | ผลที่ตรวจได้ |
| --- | --- |
| Spawn | GameMode ของแผนที่สร้าง `BP_PlayerFoundation` หนึ่งตัวที่ X=240, Z≈90.15 cm หลังลงพื้น |
| WASD | ทดสอบปุ่มจริงทั้งสี่ผ่าน `key_down` และ `key_up` W ลด X, S เพิ่ม X, A เพิ่ม Y และ D ลด Y จากมุมเริ่มต้นที่ yaw 180° |
| Mouse look | เมาส์จริงใน viewport เปลี่ยน yaw จาก 180° เป็นประมาณ -166.17° และ pitch จาก 0° เป็น -5.42° |
| กล้องบุคคลที่หนึ่ง | View target คือ `BP_PlayerFoundation_C` ตำแหน่งกล้องสูงกว่า pawn 64 cm ตาม Camera Component |
| Collision | กด W ค้างจนหยุดที่ X≈54.17 cm ด้านเดิมของผนังที่ X=0 และความสูงบนพื้นคงเดิม |
| Compile | Blueprint ทั้งสาม `UpToDate` มี 0 errors และ 0 warnings |

ใช้ Blueprint และแผนที่เดิมจากรอบพัฒนา P-01 ไม่แก้ input asset, template, project config, EngineWire หรือไฟล์ C++ กราฟ Character เหลือการเชื่อม input เดินและมองโดยตรง ยังไม่มี Blueprint Interface ในงานนี้

## ต้นแบบ Player Physical Animation · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST**

- Branch `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` งานอยู่ใน working tree และยังไม่มี review commit
- ขอบเขต Single Player อ้าง WorkPlan หน้า 9 สำหรับ Player/Object Physics แก้เฉพาะ `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation` โดยใช้ Manny และ `PA_Mannequin` เดิม ตั้ง `UpperBodyPhysicalAnimation` ตอน BeginPlay และจำลองตั้งแต่ `spine_02` ลงไปตามสายกระดูก รวมลำตัวส่วนบน คอ และแขน ขากับเชิงกรานขับด้วยแอนิเมชัน ส่วน Capsule และ Character Movement คุมการเคลื่อนที่
- Mesh ใช้ `Ragdoll` แบบ `PhysicsOnly` รับการชนกับฉากและข้าม `Pawn` กับพร็อพช่อง `PhysicsBody` เพื่อแยกแรงจาก Capsule และ Physics Handle กล้องยึด Capsule และซ่อนกระดูก `head` ตอนแสดง Manny ให้ผู้เล่นเห็น
- ปรับค่าก่อนเริ่มเล่นใน `Class Defaults → ScrubbyBoo → Physical Animation` ได้แก่ `PhysicalAnimationStrength` 0.75, `PhysicsBlendWeight` 0.55, `PositionStrength` 500, `OrientationStrength` 650, `Damping` 65, `MaximumLinearForce` 50000 และ `MaximumAngularForce` 100000 ค่า Damping ใช้กับ drive ทั้งเชิงเส้นและเชิงมุม
- Unreal MCP Compile ผ่านและบันทึก Blueprint แล้ว เปิด Blueprint และ `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` ค้างไว้ รอเมษาตรวจ gameplay ใน PIE ผู้เล่นเดียว

1. กด WASD เดิน หยุดกะทันหัน และเปลี่ยนทิศ มองลงดูการแกว่งของลำตัวและแขน ใช้เมาส์ตรวจการหันและความนิ่งของกล้อง
2. เดินเฉียดมุมผนังให้แขนสัมผัส ตรวจปฏิกิริยาของลำตัวและการเคลื่อนที่ของ Capsule
3. เล็งพร็อพ กดค้างและปล่อยปุ่มเมาส์ซ้ายและขวาทีละข้าง แล้วลองถือชิ้นเดียวกันด้วยสองปุ่มและปล่อยทีละปุ่มขณะเดินหรือหมุน

### กล้องดีบักบุคคลที่สาม · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST** · `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ยังไม่มี review commit

- `BP_PlayerFoundation` เพิ่ม `ThirdPersonDebugCamera` บน Spring Arm ระยะ 320 cm กด `V` สลับมุมมอง กล้องเดิมทำงานตั้งแต่เริ่มเล่น การสลับไปมุมดีบักแสดงหัว Manny และซ่อนอีกครั้งเมื่อกลับมุมเดิม ระบบจับของยังเล็งจากตำแหน่งกล้องบุคคลที่หนึ่ง
- Compile ผ่านและบันทึกแล้ว ใน `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` ให้เริ่ม PIE ผู้เล่นเดียว กด `V` แล้วลองเดิน หัน และจับด้วยเมาส์ซ้ายและขวา ตรวจการขยับทั้งตัว จากนั้นกด `V` กลับมุมเดิม

### ปรับท่าทางและฟิสิกส์ของ Manny · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST** · `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ยังไม่มี review commit

- แก้ `BP_PlayerFoundation` ใช้ Physical Animation Component และ `PA_Mannequin` เดิม ตั้งแรงขับและขีดจำกัดข้อต่อตอน BeginPlay ลำตัวใช้แรงคืนท่าใน world space แขนและคอใช้แรงขับตามข้อต่อแม่ หัวใช้ angular drive ของข้อต่อกับ `neck_02` เชิงกรานและขาขับด้วยแอนิเมชัน และไม่มี physics body บนกระดูกใบหน้า
- ค่าตั้งต้นใน `Class Defaults → ScrubbyBoo → Physical Animation` คือ `SpineStrength` 2.0 (ตัวคูณแรงเดิม), `ArmStrength` 12, `NeckStrength` 30, `HeadBackwardTilt` 6°, `HeadAngularLimit` 14°, `HeadDamping` 9 และ `MaximumForwardSpineBend` 24° ค่าโค้งสันหลังเป็นงบโดยประมาณ กระจายเท่ากันเป็นขีดจำกัด swing สองแกนบนสี่ข้อต่อ
- มุมบุคคลที่หนึ่งซ่อน Mesh ของผู้เล่นทั้งตัว เพราะ section ของ Manny ใช้วัสดุร่วมกันระหว่างหัวกับขา และลำตัวกับแขน กด `V` เพื่อแสดงทั้งตัวในมุมดีบัก การสลับมุมเปลี่ยนการแสดงผลของ Mesh โดยฟิสิกส์ยังทำงาน
- Unreal MCP Compile ผ่าน 0 errors และ 0 warnings บันทึกแล้ว เปิด Blueprint และ `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` ค้างไว้ รอเมษาทดสอบใน PIE ผู้เล่นเดียว

1. กด `V` ใช้ WASD เดิน หยุดเร็ว และเปลี่ยนทิศ ตรวจลำตัวตั้งตรง แขนห้อย ตลอดจนการเอียงและการตามช้าของหัว
2. ใช้เมาส์หันและกดปุ่มซ้ายหรือขวาค้างเพื่อจับพร็อพ ลองเดินหรือหมุนตอนถือ จากนั้นกด `V` กลับมุมบุคคลที่หนึ่งแล้วตรวจภาพตอนมองขึ้นและลง

### Finalize Player Prototype · จุดจับสองมือ · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST** สำหรับการเก็บจุดจับแยกมือ · `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ยังไม่มี review commit

- แก้ `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation` ผ่าน Unreal MCP และ Compile/Save ผ่าน `TryGrab` ส่งต่อ Hit เดิมทั้งตอนสร้างการจับใหม่และตอนใช้ Handle ร่วมกัน แต่ละมือเก็บ transform ของจุดกระทบในพิกัดของพร็อพหรือกระดูกที่โดนโดยไม่ trace เพิ่ม
- `GetLeftGrabContact` และ `GetRightGrabContact` คืน `HasContact` และ `WorldContact` เมื่อ Handle ยังจับอยู่ อย่างละจุดตามรอยเล็งของมือข้างนั้น ตำแหน่งและทิศอ้างอิงหมุนตามพร็อพ `ReleaseGrab` และ `UpdateGrabTargets` ใช้ Physics Handle เดิม และการปล่อยมือหนึ่งยังเก็บ Handle ไว้ถ้าอีกมืออ้างอิงอยู่
- ไม่มีพารามิเตอร์ใหม่ในขั้นนี้ `GrabDistance` และ `HoldDistance` ยังปรับได้ใน `Class Defaults → ScrubbyBoo → Grab` ค่าลำตัวเดิมยังอยู่ใน `Physical Animation`
- งานค้างคือเชื่อม `WorldContact` กับ Two Bone IK ของ Manny ลดน้ำหนักฟิสิกส์เฉพาะแขนที่จับ จำกัดระยะถึงตามความยาวแขน และแสดงแขนในมุมบุคคลที่หนึ่ง รออนุมัติ `/Game/ScrubbyBoo/Blueprints/Player/ABP_PlayerGrabIK` และ Mesh สำหรับแสดงผลใน `BP_PlayerFoundation` ก่อนแก้ส่วนนี้ ส่วนวัสดุ Manny เดิมรวมแขนกับลำตัว จึงยังซ่อน Mesh ทั้งตัวในมุมบุคคลที่หนึ่ง
- ทดสอบใน `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` แบบผู้เล่นเดียว เล็งคนละจุดบนพร็อพชิ้นเดียวแล้วกดเมาส์ซ้ายและขวาค้าง ตรวจค่าคืนจาก getter ของทั้งสองมือขณะหมุนพร็อพ จากนั้นปล่อยทีละปุ่มและตรวจว่ามือที่เหลือยังถืออยู่

### Manny Grab IK · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST** สำหรับ IK บน Manny ตัวหลัก · `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ยังไม่มี review commit

- `/Game/ScrubbyBoo/Blueprints/Player/ABP_PlayerGrabIK` ใช้ Linked Input Pose จาก locomotion เดิม แล้วใช้ Two Bone IK แยก `hand_l` และ `hand_r` โดยปิดการยืดแขนและตั้งจุดคุมศอกแยกข้าง ตำแหน่งเป้าหมายมาจาก `GetLeftGrabContact` และ `GetRightGrabContact` อัปเดตเฉพาะมือที่จับ
- `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation` ตั้ง AnimBP นี้เป็น Post Process ของ Manny ตัวหลักใน BeginPlay `SetGrabArmPhysics` หยุดจำลองเฉพาะสายกระดูกแขนข้างที่จับและคืนค่า Physical Animation ของแขนเดิมเมื่อปล่อย ใช้ Animation Update ที่มีอยู่ตามรอบของ AnimBP และไม่เพิ่ม Character Tick
- Compile ทั้งสอง Blueprint ผ่าน 0 errors และ 0 warnings และบันทึกแล้ว ไม่ได้แก้ `ABP_Unarmed` กล้อง Movement หรือการขับ Physics Handle ค่า `GrabDistance` และ `HoldDistance` ยังปรับได้ใน `Class Defaults → ScrubbyBoo → Grab` ไม่มีพารามิเตอร์ใหม่ในรอบนี้
- ทดสอบใน `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` แบบผู้เล่นเดียว กด `V` ดู Manny แล้วลองจับทีละมือและเล็งคนละจุดบนพร็อพเดียวกัน ปล่อยทีละมือเพื่อตรวจว่าแขนคืนสู่ฟิสิกส์ `HoldDistance` เดิม 170 cm อาจเกินระยะแขน เมื่อเกิน IK จะเอื้อมเต็มช่วงโดยไม่ยืดกระดูก
- ใช้ Skeletal Mesh ตัวเดียว มุมบุคคลที่หนึ่งยังซ่อน Manny สำหรับผู้เล่นเพื่อกัน Mesh บังกล้อง จึงยังไม่เห็นแขนในมุมนี้ ส่วนวัสดุของ Mesh เดิมรวมแขนกับลำตัว ถ้าต้องแสดงเฉพาะแขนให้ใช้ Mesh แขนสำหรับแสดงผลที่ไม่มี Physics หรือ Collision และอ่านจุดจับชุดเดียวกัน

## ต้นแบบ Physics Grab สองมือ · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST** · รอเมษายืนยันผล gameplay

เมษาอนุมัติให้ทำด้วย Blueprint ใน `BP_PlayerFoundation` และทดสอบใน `Lvl_Test_Player` ภายใต้ PIE ผู้เล่นเดียว ขอบเขตคือเดิน มอง จับ ถือ และปล่อยวัตถุฟิสิกส์ ตามแนว gameplay ใน WorkPlan หน้า 9 การควบคุมแยกซ้าย–ขวาอ้าง Human: Fall Flat และการถือฟิสิกส์จากมุมมองบุคคลที่หนึ่งอ้าง R.E.P.O.

- Branch `PlayerSystem` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ผลอยู่ใน working tree ยังไม่มี review commit
- `Content/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation.uasset` ใช้ `IA_Move`, `IA_MouseLook`, กล้องบุคคลที่หนึ่งและ Capsule เดิม เพิ่ม event `Pressed`/`Released` สำหรับปุ่มเมาส์ทั้งสอง ใช้ Visibility trace จากกล้องและตรวจ physics simulation ก่อนจับที่จุดโดน `LeftHandHandle` และ `RightHandHandle` เก็บสถานะมือแยกกัน เมื่อจับ Component เดียวกันจะอ้าง Handle เดียวกัน `GrabComponentAtLocation` ปล่อยให้หมุนได้ และ `UpdateGrabTargets` อัปเดตตำแหน่ง Handle ที่กำลังถือ
- ค่าเริ่มต้นสำหรับต้นแบบอยู่ใน `Class Defaults → ScrubbyBoo → Grab` คือ `GrabDistance` 325 cm และ `HoldDistance` 170 cm ปรับ damping, stiffness และ interpolation ได้ที่ `GrabHandleA` และ `GrabHandleB` ซึ่งใช้ค่าเริ่มต้นของ Unreal รอบนี้ เมื่อถือคนละชิ้น จุดถือทั้งคู่เยื้องจากกึ่งกลางข้างละ 55 cm
- `Content/ScrubbyBoo/Maps/Lvl_Test_Player.umap` เพิ่ม `GrabTest_SmallCube` 8 kg, `GrabTest_MediumBox` 25 kg และ `GrabTest_LargeObject` 60 kg ใช้ `Cube` และ `BasicShapeMaterial` ของ Engine เปิด Simulate Physics และใช้โปรไฟล์ `PhysicsActor` ทั้งสามชิ้น
- ใน Editor เปิด `BP_PlayerFoundation → EventGraph` เพื่อดูปุ่มทั้งสอง ดับเบิลคลิก `TryGrab`, `ReleaseGrab` หรือ `UpdateGrabTargets` ใน My Blueprint เพื่อดูกราฟย่อย เปิด `Lvl_Test_Player` แล้ว Play แบบผู้เล่นเดียว เล็งกลางจอที่พร็อพ กดปุ่มหนึ่งค้างไว้ กดอีกปุ่มขณะยังเล็งชิ้นเดิม แล้วสลับลำดับปล่อยปุ่ม ลองเดินและหันโดยยังถืออยู่ก่อนปล่อยมือสุดท้าย

### Compile และ checklist ทดสอบด้วยตนเอง

`BP_PlayerFoundation` สถานะ `UpToDate` มี 0 errors และ 0 warnings จากการ Compile รอบ implementation บันทึก Blueprint และแผนที่แล้ว

1. เปิด `/Game/ScrubbyBoo/Maps/Lvl_Test_Player` ใน PIE ผู้เล่นเดียว ตรวจ WASD และการหันกล้องด้วยเมาส์
2. เล็งพร็อพแล้วกดค้างและปล่อยปุ่มเมาส์แต่ละข้างแยกกัน
3. เล็งชิ้นเดิมแล้วกดทั้งสองปุ่ม ปล่อยทีละปุ่ม ตรวจว่ายังถืออยู่จนปล่อยมือสุดท้าย สลับลำดับเริ่มจับและปล่อย
4. เดินและหันขณะถือ ตรวจการแกว่ง วัตถุชนผนัง และตกลงพื้นเมื่อปล่อยมือสุดท้าย

### ประวัติการตรวจจากก่อนเปลี่ยน workflow

ข้อมูลรอบก่อนเก็บไว้เป็นประวัติ ผลรับงาน gameplay รอเมษายืนยันจาก checklist ข้างต้น

| เกณฑ์ | ผลที่ตรวจได้ |
| --- | --- |
| WASD | ส่งปุ่ม W/A/S/D ผ่าน MCP และบันทึกตำแหน่ง pawn ที่เปลี่ยนตามแต่ละปุ่มขณะถือ |
| Mouse look | ส่งค่า X/Y เข้า `IA_MouseLook` แล้วอ่าน yaw/pitch ของกล้องที่เปลี่ยนจริง เช่น yaw 180° → 150° ขณะถือ การทดสอบเส้นทางเมาส์ผ่าน viewport โดยตรงหยุดเมื่อเมษากด Escape ออกจาก Computer Use |
| จับและปล่อยแยกมือ | ตรวจทั้งลำดับซ้ายก่อนบนกล่องกลางและขวาก่อนบนลูกบาศก์เล็ก ทั้งสองอ้าง Handle เดียวเมื่อจับชิ้นเดียวกัน ปล่อยมือแรกแล้วอีกมือยังถือ และปล่อยมือสุดท้ายแล้วค่าอ้างอิงทั้งสองมือกับ Component ใน Handle กลับเป็น `None` |
| เดิน หัน ฟิสิกส์ และ collision ขณะถือ | กล่องกลางแกว่งระหว่างเปลี่ยนมุมและตำแหน่ง พอเดินเข้าผนังที่ X=0 กล่องค้างด้านเดิมที่ X≈57 cm ตรวจพร็อพทั้งสามมี Simulate Physics และ `QueryAndPhysics` ใน PIE |
| ตกหลังปล่อย | กล่องกลางลดจาก Z≈133 cm ลงมาถึง Z≈60 cm ในการวัด 1.2 วินาทีหลังปล่อย และเกิดการกระทบพื้น |
| Compile | `BP_PlayerFoundation` สถานะ `UpToDate` มี 0 errors และ 0 warnings จาก Unreal MCP และบันทึก asset กับแผนที่แล้ว |

## ต้นแบบ Cleaning Tools (Mop, Cobweb Broom, Garbage Bag) · 27 พฤศจิกายน 2026

**IMPLEMENTED - PENDING MANUAL TEST** · Claude Code · รอเมษายืนยันผล gameplay

- Branch `CleaningSystem` ใน worktree `C:/Users/masap/GameLab-Cleaning` ฐาน `2a8f5504bb27e21e66ad13cd4b215afb19e4bbe2` ยังไม่มี review commit Editor แยกของ worktree เปิดด้วย `MCP_NATIVE_PORT=3001` ไม่ได้แตะ worktree หรือ Editor ของ Codex
- Asset ใหม่ทั้งหมดอยู่ใน `/Game/ScrubbyBoo/Blueprints/Cleaning/` (โฟลเดอร์ที่อนุมัติ) และ `/Game/ScrubbyBoo/Maps/Lvl_Test_Cleaning`
  - `BP_CleaningTool` (parent StaticMeshActor, root ฟิสิกส์) มี `HeadCleanZone`, `CleanTargetClass`, `Holder`, `HeldHandCount`, `bCleaningActive` และฟังก์ชัน `NotifyGrabbed`, `NotifyReleased`, `TryCleanActor`, `CleanOverlapping`, `IsOtherToolActiveForHolder`
  - `BP_Mop` (ทำความสะอาด `BP_Stain`), `BP_CobwebBroom` (ทำความสะอาด `BP_Cobweb`), `BP_GarbageBag` (ไม่มี contact cleaning มี `CollectGarbage(Target)` และ `CollectRange` 300 cm)
  - `BP_Stain`, `BP_Cobweb`, `BP_Garbage` เป็น dirt แบบ placeholder จาก BasicShapes
  - **ชั่วคราว:** `BP_CleaningTestPawn` (ลูกของ template `BP_FirstPersonCharacter`) และ `BP_CleaningTestGameMode` ใช้ทดสอบแยกจาก Player จริงเท่านั้น
- กติกา: ทำความสะอาดเมื่อ `bCleaningActive` และ dirt เป็นคลาสที่ตรงกับอุปกรณ์ ม็อบ/ไม้กวาดทำงานด้วย overlap ของหัวเท่านั้น ด้ามไม่มีผล ถุงเก็บขยะเฉพาะเมื่อกด Interact ภายใน 300 cm ไม่เก็บด้วย overlap ผู้ถือคนเดียวมีอุปกรณ์ทำงานได้หนึ่งชิ้น สองมือบนอุปกรณ์เดียวนับ `HeldHandCount` ไม่ทำงานซ้ำ ปล่อยมือสุดท้ายแล้วหยุดทันที dirt ที่ทำความสะอาดแล้วถูกลบจากฉาก ไม่มีเวลา ความทนทาน ความจุ หรือคะแนน
- Compile ล่าสุด: 9 Blueprints ใน Cleaning folder, 0 failed, 0 flagged บันทึก asset และแผนที่แล้ว เปิด `BP_CleaningTool`, `BP_GarbageBag`, `BP_CleaningTestPawn` และ `Lvl_Test_Cleaning` ไว้ใน Editor ของ worktree
- ก่อน workflow Implementation Only มีผล ได้รันการตรวจ logic ผ่าน MCP ใน PIE ผู้เล่นเดียวแล้ว (ถือ/ไม่ถือ, เครื่องมือผิดชนิด, ปล่อยแล้ว, หนึ่งชิ้นต่อผู้ถือ, สองมือ, overlap ถุงกับขยะ, ปุ่ม G/E ของ test pawn) ผลทุกข้อตรงตามคาด ถือเป็นข้อมูลประกอบ ผล gameplay รอเมษายืนยัน

### เชื่อมกับ `BP_PlayerFoundation` · IMPLEMENTED - PENDING MANUAL TEST

- เมษาอนุมัติให้ merge `PlayerSystem` (`30c4a0a`) เข้า `CleaningSystem` ได้ merge commit `6a4f7e0` แล้ว build Editor target ของ worktree ผ่าน ส่วน integration ยังไม่ commit
- `BP_PlayerFoundation` เพิ่ม 3 ฟังก์ชัน โดยไม่แก้ `TryGrab`, `ReleaseGrab`, `UpdateGrabTargets`, กล้อง, IK หรือ body physics
  - `GetHandleCleaningTool(Handle)` คืน `BP_CleaningTool` จาก component ที่ Physics Handle จับอยู่
  - `NotifyCleaningToolHand(Handle, bGrabbed)` เรียก `NotifyGrabbed`/`NotifyReleased` บนอุปกรณ์ต่อมือหนึ่งครั้ง
  - `CollectWithBag(Bag)` trace จาก `FirstPersonCamera` ตาม `GrabDistance` โดยไม่โดนตัวเองและถุง แล้วเรียก `CollectGarbage`
- EventGraph: แจ้งหยิบหลัง `Set Left/RightGrabLocalContact` และแจ้งปล่อยก่อน `ReleaseGrab` ของเมาส์แต่ละข้าง ปุ่ม **E** (ค่าชั่วคราว) หา `BP_GarbageBag` จากมือซ้ายก่อนแล้วมือขวา ไม่มี Tick ใหม่
- `Lvl_Test_Cleaning` ใช้ GameMode Override `BP_PlayerFoundationTestGameMode` ลบ `BP_CleaningTestPawn` และ `BP_CleaningTestGameMode` ที่ไม่ใช้แล้ว (ยังอยู่ใน `e7ade44`)
- Compile: `BP_PlayerFoundation` 0 errors, 0 warnings และ Cleaning folder 7 Blueprints 0 failed บันทึกแล้ว
- ข้อจำกัด: Player cast ไปยัง `BP_CleaningTool`/`BP_GarbageBag` โดยตรงจนกว่า interface ร่วมจะได้รับอนุมัติ

### Progressive Physical Cleaning · IMPLEMENTED - PENDING MANUAL TEST

- **Dirt** (`BP_Stain`, `BP_Cobweb`): `CleaningProgress` 0–100 (ค่าเริ่มต้น 0) และ `DirtColor` สร้าง Dynamic Material Instance ครั้งเดียวตอน BeginPlay จาก `M_DirtPlaceholder` (`/Game/ScrubbyBoo/Art/Environment/`, Translucent, scalar `Opacity`, vector `DirtColor`) `AddCleaningProgress(Amount)` clamp 0–100 แล้วตั้ง Opacity = 1 − CleaningProgress/100 เฉพาะตอนค่าเปลี่ยน เมื่อถึง 100 ปิด collision และซ่อน actor ปิด `bCanEverTick`
- **Tool** (`BP_CleaningTool`): เก็บ dirt ที่ `HeadCleanZone` overlap อยู่ใน `CurrentDirt` (เพิ่มตอน BeginOverlap ลบตอน EndOverlap) `UpdateCleaningTimer` เปิด timer แบบ looping ทุก `CleaningInterval` เฉพาะเมื่อถืออยู่และมี dirt สัมผัส และหยุดเมื่อไม่ตรงเงื่อนไข `CleaningTick` วัดการเคลื่อนที่จริงของ `HeadCleanZone` ระหว่างรอบ (ม็อบวัดเฉพาะแนวพื้น XY ไม้กวาดวัด 3 แกน) เมื่อความเร็ว ≥ `MinimumCleaningSpeed` เพิ่ม progress = `CleaningRate` × เวลาที่ผ่านไปจริง progress อยู่บน dirt จึงคงไว้เมื่อหยุด ปล่อย หรือเดินออก ไม่มี Tick
- **Details** (`Cleaning|Balancing`): `CleaningDuration` 4 s, `CleaningRate` 25 /s, `CleaningInterval` 0.1 s, `MinimumCleaningSpeed` 15 cm/s, `bMeasureFloorSpeedOnly` (ม็อบ true, ไม้กวาด false) ปรับแยกใน `BP_Mop` และ `BP_CobwebBroom` ได้ เมื่อ `CleaningDuration` > 0 ตอน BeginPlay จะคำนวณ `CleaningRate` = 100 / `CleaningDuration` ค่าเริ่มต้นจึงตรงกัน (4 s → 25 /s)
- ถุงขยะยังเก็บด้วยปุ่ม E เท่านั้น สองมือ: ปล่อยมือหนึ่งยังทำความสะอาดต่อจนปล่อยมือสุดท้าย
- ระหว่างทำงาน การ compile `BP_GarbageBag` ใหม่ทำให้ input ที่ระบุชนิดเป็น `BP_GarbageBag` ใน `CollectWithBag` ของ `BP_PlayerFoundation` เสีย จึงสร้างฟังก์ชันเชื่อมทั้งสามใหม่ และให้ `CollectWithBag` รับ `Actor` แล้ว cast ภายใน ตรวจแล้วว่า compile Cleaning → Player สลับกันสองรอบผ่านทั้งหมด การ refresh Blueprint ในโฟลเดอร์ Player บันทึก `ABP_PlayerGrabIK` ใหม่ด้วย โดยไม่ได้แก้ logic
- Compile ล่าสุด: Cleaning 7 Blueprints 0 failed และ `BP_PlayerFoundation` 0 errors, 0 warnings บันทึกแล้ว `Lvl_Test_Cleaning` บันทึกแล้ว ข้อจำกัด: สีของ dirt เป็น placeholder ยังไม่มี texture จริง

### Cleaning Tool Handling (ฝั่งเครื่องมือ) · IMPLEMENTED - PENDING MANUAL TEST · ฝั่ง Player รออนุมัติ

- `BP_CleaningTool` เพิ่ม `GripPointA`, `GripPointB` (Scene Component) และ `GrabAssistCollision` (Capsule) **แก้ไขแล้ว:** เดิมตั้ง preset `UI` ให้บล็อก Visibility ทำให้ line trace ของ `TryGrab` โดน capsule ก่อน capsule เป็น query-only จึงไม่ weld กับ body ฟิสิกส์ `IsSimulatingPhysics` ของ component ที่โดนเป็น false และ `TryGrab` ปฏิเสธการจับ ตอนนี้ตั้ง capsule เป็น `NoCollision` การจับกลับเป็นเหมือนก่อนเพิ่ม capsule การเล็งง่ายขึ้นต้องรอ Sphere Trace ฝั่ง Player (ข้อ 1 ที่รออนุมัติ)
- ถือมือแรก: `ApplyHeldPhysics` เก็บ angular damping เดิมแล้วตั้ง `HeldAngularDamping` ปล่อยมือสุดท้าย: `RestoreHeldPhysics` คืนค่าเดิม Simulate Physics และ collision คงเดิม ไม่มี Tick
- ฟังก์ชันให้ Player เรียก: `GetGripLocation(NearLocation)` คืน grip ที่ใกล้จุดเล็ง และ `GetDesiredHoldRotation(ViewRotation)` คืนทิศที่ควรถือ (`OrientationMode` 1 = ม็อบ ด้ามเอียงเข้าหาผู้เล่น `HandleTiltAngle`, 2 = ไม้กวาด ด้ามชี้ตามทิศเล็ง, 0 = ถุง ห้อยตามแรงโน้มถ่วง)
- Details `Cleaning|Grip`: `HeldAngularDamping` 8, `RotationStrength` 1500, `GrabAssistRadius` 40, `OrientationMode`, `HandleTiltAngle` 15 ตำแหน่ง grip: ม็อบ 10 และ 50 cm ใต้ปลายด้าม, ไม้กวาด 20 และ 70 cm จากปลายด้าม, ถุง ด้านบนใกล้ปากถุง
- **รออนุมัติแก้ `BP_PlayerFoundation`:** (1) `TryGrab` ใช้ Sphere Trace สำรองเมื่อ line trace พลาด และรับเฉพาะ `BP_CleaningTool` (2) เมื่อจับเครื่องมือ ให้จับที่ `GetGripLocation` และถ้า `bUseAssist` ใช้ `GrabComponentAtLocationWithRotation` กับตั้ง Angular Stiffness ของ handle เป็น `RotationStrength` (3) `UpdateGrabTargets` เดิมตั้ง target rotation จาก `GetDesiredHoldRotation` เฉพาะ handle ที่ถือเครื่องมือ สองมือใช้ handle เดียวกันจึงมี rotation drive เดียว (4) ปล่อยมือสุดท้ายคืนค่า stiffness เดิมของ handle ของทั่วไปยังจับแบบเดิม
- Compile: Cleaning 7 Blueprints 0 failed, `BP_PlayerFoundation` compile ซ้ำผ่าน (ไม่ได้แก้)

### ปุ่มควบคุมใน `Lvl_Test_Cleaning`

WASD/เมาส์ตาม Player · **เมาส์ซ้าย/ขวา** กดค้างเพื่อจับด้วยมือซ้าย/ขวา ปล่อยปุ่มเพื่อวาง · **E** Interact เก็บขยะเมื่อถือถุง (ค่าชั่วคราว) · **V** กล้อง debug ของ Player

### Checklist ทดสอบด้วยตนเอง

1. จับม็อบแล้วลากหัวม็อบผ่านคราบ คราบหายไป ด้ามแตะคราบแล้วไม่หาย
2. จับไม้กวาด เงยหน้าให้หัวไม้กวาดแตะหยากไย่ที่ผนังและเพดาน หยากไย่หายไป
3. จับถุง เล็งขยะใกล้ ๆ กด E ขยะหายไป เอาถุงไปชนขยะเฉย ๆ ไม่เก็บ
4. ใช้อุปกรณ์ผิดชนิด ไม่มีผล ปล่อยอุปกรณ์ให้ตกใส่ dirt ไม่มีผล
5. จับชิ้นเดิมสองมือ ปล่อยมือหนึ่งยังทำงาน ปล่อยมือสุดท้ายหยุดทำงาน จับอุปกรณ์อีกชิ้นด้วยมืออีกข้าง ชิ้นที่สองไม่ทำงาน

## ผลรีวิวสถาปัตยกรรมโดย Claude Code · รอเมษาตัดสิน

บันทึกประวัติการรีวิวแผนเดิม ข้อเสนอในส่วนนี้ไม่ใช่ขอบเขต P-01 Single Player และต้องขออนุมัติแยกก่อนนำไปใช้

รีวิว 27 พฤศจิกายน 2026 บน `PlayerSystem` `2a8f550` (working tree ยังไม่ commit) เทียบซอร์สจริง, WorkPlan V1 หน้า 9–11, Technical Architecture v0.1, FigJam Architecture v0.1 และแผน P-01/สัญญาร่วมฉบับล่าสุด ยังไม่ได้ build หรือเปิด Editor ข้อเสนอด้านล่างต้องให้เมษาอนุมัติก่อนนำไปแก้ task

| ID | ข้อค้นพบ | ข้อเสนอให้แก้ก่อนลงมือ |
| --- | --- | --- |
| R-01 · วิกฤต | สัญญาร่วมห้าม include/cast ข้าม feature แต่ยังไม่มีที่วาง declaration ที่ใช้ร่วมกันได้นอก `Shared/` จึงยังเชื่อม Player กับ Cleaning ด้วย C++ ไม่ได้ | ขออนุมัติเจ้าของ `Shared/` สำหรับ interface + struct บริบทหนึ่งชุด กำหนดผู้เขียนคนเดียวใน PC-01 |
| R-02 · วิกฤต | `CanApplyCleaningInteraction`/`ApplyCleaningInteraction` ทำให้ Player ต้องรู้จัก Cleaning และ `ResolveInteractionContext` เป็นขั้นตอนภายใน Player ไม่ใช่สัญญา | ใช้ interface กลางแบบเดียวกับ `UInteractableInterface` ใน Technical Architecture ให้เป้าหมาย/อุปกรณ์ Cleaning implement เอง Player ส่งเพียงบริบทที่ server ตรวจแล้ว (instigator, held actor, hit component, hit location) |
| R-03 · วิกฤต | สัญญามีแค่การกระทำครั้งเดียว แต่การถูคราบและกวาดหยากไย่เป็นงานต่อเนื่อง ม็อบต้องเก็บ progress เมื่อพัก (หน้า 9) และการสวดคุมระยะพักการถูได้ (หน้า 11) | เพิ่มคู่ BeginUse/EndUse ที่ส่งผ่าน `Server_` RPC ให้ server สะสม progress ตามเวลา ห้ามส่ง RPC ทุกเฟรม และให้ระบบภายนอกสั่งพักการใช้งานได้ |
| R-04 · วิกฤตสำหรับ P-03 | ยังไม่ได้เลือก network model ของการถือ Physics Handle ไม่ replicate เอง ถ้าจำลองบน server อย่างเดียว ผู้ถือฝั่ง client จะเห็นของหน่วง | ระบุใน P-03 ว่า server จำลองของที่ถือแล้ว replicate movement ส่วนผู้ถือทำ smoothing/prediction เฉพาะภาพ หรือเลือกโหมด physics replication ของ UE 5.6 ที่ต้องเปิดใน Config (ต้องขอเจ้าของ Config) |
| R-05 | ยังไม่มีทางให้ระบบอื่นบังคับปล่อยของ แต่ WorkPlan ให้ชน/ล้มแล้วของหลุดมือ (หน้า 9) และให้ผีทำม็อบหลุดหรือแย่งอุปกรณ์ไปตกใกล้เท้า (หน้า 10) | เพิ่ม API ฝั่ง server สำหรับ force release พร้อมเหตุผลและจุดตก ใน component การถือ |
| R-06 | ระยะ trace ของ Interaction ถูกกำหนดตายตัวที่ฝั่ง Player แต่ไม้กวาดต้องเอื้อมถึงเพดาน (หน้า 9) และอัปเกรดทำให้ด้ามยาวขึ้น/หัวม็อบกว้างขึ้น (หน้า 11) | ให้อุปกรณ์ที่ถืออยู่ส่งค่าระยะเอื้อมผ่าน interface และให้ server ใช้ค่าเดียวกันตอนตรวจ |
| R-07 | หลายระบบจะแก้ความเร็วเดินพร้อมกัน ทั้งน้ำหนักของที่ถือ คนลากเดินช้าลง วิ่ง และผียื้อ (หน้า 9–10) ถ้าเปลี่ยน `MaxWalkSpeed` ฝั่ง server อย่างเดียวจะเกิด correction | ให้ Player รวม speed modifier จุดเดียว คำนวณจาก state ที่ replicate แล้วทั้งสองฝั่ง ออกแบบใน P-01 หรือ P-03 |
| R-08 | P-04 ยังไม่ระบุ network model ของ ragdoll และ Technical Architecture ยังไม่ได้ทดสอบว่า ragdoll replicate ได้ | replicate เฉพาะ physics state กับเหตุแรงกระแทก ให้แต่ละเครื่องจำลอง ragdoll เป็นภาพ capsule ยังเป็นตำแหน่งจริงของ server และใช้ physical animation ระหว่างเอื้อม/ถือเป็นภาพเฉพาะเครื่อง |
| R-09 | ชื่อในแผน P-01 (`AScrubbyPlayerCharacter`, `BP_PlayerFoundation`) ต่างจาก `AScrubbyCharacter` ใน Multiplayer Architecture และ `BP_ScrubbyCharacter` ใน Team Workflow ถ้าเปลี่ยนชื่อภายหลังต้องเพิ่ม CoreRedirects ใน `Config/` | ให้เมษาเลือกชื่อถาวรก่อน P-01 ไม่ใช้ชื่อตามเฟส เช่น Foundation กับ pawn ที่จะใช้ต่อ |
| R-10 | ชื่อและที่อยู่ของการถือยังไม่ตรงกัน Technical Architecture ใช้ `UPlayerHoldComponent` ส่วน Multiplayer Architecture ใช้ `UGrabComponent` และ Team Workflow ใช้ `Systems/Grab/` | ใช้ชื่อเดียวใน `Systems/Grab/` และยืนยันกับเมษาก่อน P-03 |
| R-11 | `GetCleaningState` ระบุ Player เป็นผู้อ่าน แต่ Player ไม่ต้องใช้ ผู้อ่านจริงคือเงื่อนไขออกด่านและ UI ในอนาคต | ตัด Player ออกจากผู้อ่าน ให้เก็บความคืบหน้ารวมบน replicated actor ของ Cleaning ที่ relevant กับทุกคน เลี่ยงการแก้ GameState ใน `Core/` |
| R-12 | INTERFACES ใช้แจ้ง `OnHeldActorChanged` ผ่าน delegate ของ Player ซึ่งทำให้ Cleaning ต้อง bind ข้าม feature | แจ้งของที่ถูกถือผ่าน interface บน held actor เอง (picked up/dropped) อุปกรณ์ Cleaning จึงรับรู้ได้โดยไม่ต้องรู้จัก Player |

ลำดับที่เสนอ: P-01 → PC-01 (header draft จริง + อนุมัติ `Shared/`) → P-02 → P-03 → C-01/C-02 ด้วย fixture → QA-01 → P-04 → P-05 → P-06 ให้ Cleaning ทดสอบร่วมได้ก่อนถึงงาน physics ของร่างกายที่ยังมีค่าเปิดอยู่มาก

Input assets ที่ยังขาด: Interact/หยิบ, ปล่อย, ใช้อุปกรณ์ และวิ่ง ส่วน template มีเพียง Move/Look/MouseLook/Jump ต้องเลือกปุ่มจริงและที่เก็บ asset เพราะ `Content/ScrubbyBoo/` ยังไม่มีโฟลเดอร์ Input

## ขอบเขตที่รอข้อสรุป

- การเลือกปุ่มจริง ค่าแรง/มวล/ระยะ ค่า recovery การยกสองคน การคืนอุปกรณ์ที่เอื้อมไม่ถึง และการใช้เศษจากของแตกเป็นงานเพิ่ม มีรายละเอียดที่ต้องตกลงตาม WorkPlan หน้า 9
- UI, death/revival, เศรษฐกิจ, combat/upgrade/ตราขับผี, Ghost AI, Voice Interaction, ระบบ Lobby/Steam และเงื่อนไขออกด่านฝั่ง GameMode อยู่นอก backlog ปัจจุบัน

## ข้อมูลส่งมอบต่อ task

- ID, ผู้รับผิดชอบ, branch, base commit และ review commit/diff
- ไฟล์ที่อนุญาตและไฟล์ที่เปลี่ยนจริง
- ฟีเจอร์ที่ทำ ผล Compile และสถานะ `IMPLEMENTED - PENDING MANUAL TEST` จนกว่าเมษาจะยืนยันผล gameplay
- Test level path, ปุ่มควบคุม, checklist ทดสอบด้วยตนเองแบบสั้น และข้อจำกัดที่ทราบ
