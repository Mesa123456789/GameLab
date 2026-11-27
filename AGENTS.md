# Scrubby Boo! · ข้อตกลงสำหรับ Codex และ Claude Code

## ขอบเขตงานของเมษา

เมษา (Mesa) เป็น Gameplay Developer ของ `ScrubbyBooGame` บน Unreal Engine 5.6.1 งาน gameplay ของเมษาใช้ **Blueprint เท่านั้น** โชต (Chot) รับผิดชอบการพัฒนา C++ ให้ AI ใช้คลาสเดิมและ API ที่ expose ให้ Blueprint ได้ตามขอบเขต ห้ามสร้างหรือแก้ไฟล์ C++ หาก Blueprint ที่มีอยู่ขาดความสามารถที่จำเป็น ให้บันทึก dependency เพื่อให้เมษาประสานโชต

| ผู้รับผิดชอบ | ขอบเขตปัจจุบัน |
| --- | --- |
| Codex | Player Foundation, Player Movement, Player Interaction, Grab and Hold และ Player/Object Physics ในส่วนที่เมษารับผิดชอบ |
| Claude Code | Cleaning Tools, Cleaning Targets และ Cleaning Progress |

พัก UI และสงวนระบบของโชตกับผู้พัฒนาคนอื่น รวม Ghost AI, Voice Interaction และ Multiplayer infrastructure

`Core/`, `Multiplayer/`, `Shared/`, `Config/`, `.uproject`, `.Build.cs`, `*.Target.cs`, Template/Variants, Blueprint กลาง และแผนที่กลางอยู่ภายใต้เจ้าของเดิม `Shared/` ต้องได้รับอนุญาตจากเจ้าของก่อนแตะ สร้างหรือแก้ Blueprint/แผนที่ผ่าน Unreal Editor หลังเมษาอนุมัติ asset และขอบเขต task เท่านั้น ห้ามแก้ `.uasset` หรือ `.umap` เป็นไฟล์ไบนารีโดยตรง หากเครื่องมือเข้าถึง graph ไม่ได้ ให้ส่งขั้นตอนทีละโหนดและค่าที่ต้องตั้งให้เมษา

## เอกสารที่ใช้อ้างอิง

- `Scrubby Boo!_WorkPlan Version 1.pdf` เป็นแหล่งอ้างอิงสำหรับ gameplay ที่ยืนยันแล้ว ดูตำแหน่ง PDF และความขัดแย้งใน [STATUS](Docs/AI/STATUS.md) แยกข้อยืนยันออกจากค่าทดลอง ข้อเสนอ และประเด็นที่ยังไม่มีข้อสรุป แม้อยู่ในแถวที่ระบุว่า “คุยผ่านแล้ว”
- ใช้ [Multiplayer Architecture](Docs/Docs/MultiplayerArchitecture.md) สำหรับ authority, replication และ Blueprint handover และ [Team Workflow](Docs/Docs/TeamWorkflow.md) สำหรับโครงสร้างโฟลเดอร์ การแบ่งไฟล์ และการตั้งชื่อ ใช้กติกา Git ด้านล่างสำหรับงานของเมษาเมื่อแนวทางตั้งชื่อหรือจัดการ branch ในคู่มือแตกต่างกัน
- ใช้ [Technical Architecture v0.1](Docs/Docs/ScrubbyBoo_TechnicalArchitecture.docx) เป็นข้อเสนอทางเทคนิค ขอบเขตและการแบ่งงาน AI ให้ยึดเอกสารฉบับนี้และคำสั่งล่าสุดของเมษา
- อ่าน requirement ที่อนุมัติจาก [STATUS](Docs/AI/STATUS.md) และ [TASKS](Docs/AI/TASKS.md) ตามส่วนที่เกี่ยวข้อง ใช้ [INTERFACES](Docs/AI/INTERFACES.md) เมื่องานต้องอาศัยสัญญาร่วม ใช้ข้อมูลที่อ่านแล้วต่อได้เมื่อเอกสารไม่เปลี่ยน หากเอกสารถูกย้าย ให้ตรวจ path จริงจาก working tree และ Git

## กติกา Git และการทำงานร่วมกัน

- งาน Player ทั้งหมดใช้ `PlayerSystem` เดิม ส่วนงาน Cleaning จะใช้ `CleaningSystem` เมื่อเมษาหรือทีมจัดเตรียมและยืนยัน base branch กับ dependencies แล้ว Codex และ Claude Code พัฒนาคนละระบบได้อย่างอิสระผ่านสัญญาที่ตกลงใน PC-01 และ fixture ของตน ห้าม AI สร้าง branch หรือ worktree เอง
- ตรวจ current branch, base commit และ Git status ก่อนแก้ไขทุกครั้ง รักษาไฟล์ที่ผู้พัฒนาคนอื่นแก้ค้างไว้ ห้ามสลับ branch, merge, rebase, push, force push หรือลบ branch โดยอัตโนมัติ ให้ Git Manager ดูแลการ merge เข้า `main`
- เมื่อใช้ working directory เดียวกัน ให้ Codex กับ Claude Code ผลัดกันแก้ไฟล์และส่งมอบสถานะก่อนเริ่มรอบถัดไป ห้ามสลับ branch ขณะที่อีกคนกำลังแก้ไฟล์ ให้เมษาอนุมัติ task, acceptance criteria และไฟล์ที่อนุญาตก่อนเริ่ม implementation

## กติกาโครงสร้างโฟลเดอร์

- ยึดโครงสร้างที่มีอยู่จริงและ Team Workflow ก่อนสร้างไฟล์ใด ๆ ให้ตรวจโฟลเดอร์ คลาส โมดูล interface และการตั้งชื่อที่มีอยู่ ใช้โฟลเดอร์และโค้ดเดิมเมื่อเหมาะสม ห้ามสร้างระบบ คอมโพเนนต์ interface หรือ utility ซ้ำ และห้ามใช้ไฟล์ของระบบที่ผู้พัฒนาคนอื่นดูแลโดยพลการ
- ห้ามสร้างโฟลเดอร์ตามข้อเสนอทางสถาปัตยกรรมโดยพลการ โฟลเดอร์เฉพาะ AI หรือโครงสร้างซ้อนซ้ำอย่าง `Docs/Docs/` ห้ามจัดโครงสร้างใหม่ เปลี่ยนชื่อ ย้าย หรือลบโฟลเดอร์เดิมโดยไม่ได้รับอนุมัติ
- หากต้องใช้โฟลเดอร์ใหม่จริง ให้เสนอ path, จุดประสงค์, เหตุผลที่ใช้โฟลเดอร์เดิมไม่ได้ และรายการไฟล์ที่คาดว่าจะอยู่ในโฟลเดอร์ จากนั้นรอเมษาอนุมัติก่อนสร้าง

## Workflow · Implementation Only

ใช้กับ Codex และ Claude Code ตั้งแต่งานปัจจุบันและทุกงานพัฒนาถัดไป เมษาเป็นผู้ทดสอบ gameplay ทั้งหมด

1. อ่านเฉพาะ requirement ที่อนุมัติและทำเฉพาะฟีเจอร์ที่ขอ ใช้ Unreal MCP สร้างหรือแก้ Blueprint ใช้ความสามารถเดิมของ Unreal และทำกราฟให้อ่านง่าย รวมคำสั่ง MCP เป็น batch เมื่อรองรับ
2. Compile Blueprint ที่แก้หนึ่งครั้งหลัง implementation ถ้ามี Compile error ให้อ่านเฉพาะข้อมูลที่จำเป็น แก้แล้ว Compile ซ้ำเพื่อยืนยันการแก้ บันทึก asset ที่แก้ทั้งหมด แล้วเปิด test level และ Blueprint ที่เกี่ยวข้องให้เมษาตรวจใน Editor เมื่อ MCP รองรับ
3. ใช้การตรวจ asset กราฟ ซอร์ส หรือ log เท่าที่จำเป็นต่อ implementation หรือแก้ Compile error ใช้แนวทางปัจจุบันจนกว่าจะติดขัด หากการดำเนินการเดิมล้มเหลวสองครั้ง ให้รายงาน blocker แล้วหยุดลองซ้ำ
4. งด automated gameplay test, PIE test, input simulation, Computer Use สำหรับทดสอบ, การเก็บภาพ viewport ซ้ำ, การตรวจซ้ำเป็นวงกว้าง, MCP plugin debugging, multiplayer/networking และ optimization หรือ refactor ที่ไม่ได้ขอ
5. เมื่อ Compile ผ่านและบันทึกงานแล้ว ให้ลงสถานะ `IMPLEMENTED - PENDING MANUAL TEST` ใน `Docs/AI/` รอผลจากเมษาก่อนระบุว่า gameplay ผ่าน ใช้แผนและอัปเดตความคืบหน้าสั้น ๆ
6. ส่งมอบเป็นภาษาไทยเฉพาะฟีเจอร์ที่ทำ รายการ asset ที่สร้างหรือแก้ path ของ test level ปุ่มควบคุม checklist ทดสอบด้วยตนเองแบบสั้น และข้อจำกัดที่ทราบ จากนั้นหยุดและรอผลจากเมษาก่อนเริ่มงานอื่น

บันทึก task และ branch/base/review commit ตามจริงใน `Docs/AI/` ประสานผู้แก้เอกสารกลางก่อนแก้ไฟล์เดียวกัน ใช้ชื่อโค้ดและ asset ภาษาอังกฤษตาม Team Workflow

## ขอบเขต P-01 ปัจจุบัน

P-01 ที่อนุมัติครอบคลุม Single Player เมษาตรวจ spawn, WASD, mouse look, กล้องบุคคลที่หนึ่ง และ collision ใน Standalone PIE หนึ่งผู้เล่น ส่วน AI รับผิดชอบ Compile Blueprint

## มาตรฐาน Blueprint และแนวทางสำหรับงาน Multiplayer ที่อนุมัติแยก

แนวทางด้านล่างเป็นข้อมูลอ้างอิงเดิมสำหรับงานที่ต้องมีคำสั่งแยก ภายใต้ workflow ปัจจุบัน AI งดงาน multiplayer และ networking

- ใช้ `Character`/Character Movement, Enhanced Input และ Blueprint API ที่มีอยู่ก่อนเพิ่ม graph หรือ component แยก input และกล้องไว้กับ local player ด้วย `Is Locally Controlled` หรือ `Is Local Controller` ให้ Character Movement จัดการ prediction และ replication ตามเดิม
- ให้ server เปลี่ยน gameplay state เมื่อมี client request ใช้ Blueprint Custom Event แบบ `Run on Server` บน actor ที่ client เป็นเจ้าของ ตรวจสิทธิ์ เป้าหมาย ระยะ และ state ฝั่ง server ใช้ replicated variables หรือ RepNotify สำหรับ state ที่ผู้เล่นและ late join ต้องเห็น ใช้ Multicast สำหรับ cosmetic ชั่วคราว เมษาทดสอบ Host และ Client จากเส้นทางเดียวกัน
- Player dispatch การปฏิสัมพันธ์แบบทั่วไปโดยไม่รู้กติกาอุปกรณ์ Cleaning และ Cleaning ใช้ `Pawn`/`Actor` กับ Blueprint Interface โดยไม่ cast ไปยัง Blueprint Player ตัวจริง ตกลงผู้ดูแล path, input, output และ authority ใน [INTERFACES](Docs/AI/INTERFACES.md) ก่อนสร้าง interface asset
- ใช้ Blueprint comments และหมวด `ScrubbyBoo|<System>` ตามเหมาะสม บันทึกผล Compile ตามจริง และใช้ผล PIE ที่เมษายืนยัน ใช้ [.agents/skills/mp-bp-handover](.agents/skills/mp-bp-handover/SKILL.md) เฉพาะแนวทาง authority, replication และ handoff ที่เข้ากับขอบเขต Blueprint งาน C++ และเอกสารส่วนกลางให้โชตหรือเจ้าของรับผิดชอบ

## Unreal MCP สำหรับงานที่อนุมัติ

- เปิดโปรเจกต์ด้วยตัวเปิดบน Desktop `ScrubbyBooGame - EngineWire.lnk` รอ Editor โหลด แล้วตรวจการเชื่อมด้วย `unreal` ที่ `operation=execute`, `capability=manage_level.list_levels`, `params={}` ตรวจว่าเป็น ScrubbyBooGame และโลกที่ต้องการ ถ้าเครื่องมือไม่พร้อม ให้ระบุ blocker
- ใช้ Unreal MCP เป็นช่องทางหลักสำหรับ Blueprint และการทำงานใน Editor ใช้ `search` และ `describe` ตรวจ capability, inputs และข้อจำกัดก่อนแก้ ใช้ terminal/filesystem สำหรับ Git, logs และงานสนับสนุน เก็บ token ไว้ในเครื่องและห้ามใส่ค่าในเอกสารหรือ Git
- ใช้ขั้นตอน Compile, Save และเปิด asset ตาม Implementation Only หาก MCP ทำขั้นตอนที่จำเป็นไม่ได้ ให้รายงานข้อจำกัดเฉพาะขั้นตอนนั้น
- เวอร์ชัน ตัวเปิด พอร์ต และวิธีแก้ Movie Render Pipeline อยู่ใน [STATUS](Docs/AI/STATUS.md#unreal-mcp-ที่ตรวจแล้ว)

## กติกา gameplay

อ้าง WorkPlan พร้อมเลขหน้าสำหรับงานที่เสนอ ใช้ Manny ชั่วคราวโดยแยก body configuration ออกจาก movement, interaction, physics และ Cleaning ขอข้อสรุปจากเมษาและผู้รับผิดชอบก่อนเริ่มส่วนที่ต้องใช้กติกาที่ยังเปิดอยู่ เช่น ค่าแรงชน เวลา recovery เกณฑ์เศษนับเป็นงานเพิ่ม และ death/revival
