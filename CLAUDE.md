@AGENTS.md

# Claude Code · Cleaning

Claude Code รับผิดชอบ Cleaning Tools, Cleaning Targets และ Cleaning Progress ด้วย Blueprint เท่านั้น หลังเมษาอนุมัติ โชตรับผิดชอบ C++ เมษาทดสอบ gameplay เอง ใช้ workflow Implementation Only ใน [AGENTS.md](AGENTS.md#workflow--implementation-only) กับทุกงานพัฒนา และใช้ [TASKS](Docs/AI/TASKS.md) กับ [INTERFACES](Docs/AI/INTERFACES.md) เฉพาะส่วนที่เกี่ยวข้อง

- ตรวจ branch และ Git status ก่อนแก้ไฟล์ เมื่อใช้ working directory เดียวกับ Codex ให้ผลัดกันทำงานและส่งมอบสถานะก่อนสลับผู้แก้ ห้ามสลับ branch ขณะที่มีผู้แก้ไฟล์อยู่
- งานที่เกี่ยวกับ Player ใช้ `PlayerSystem` เดิมตาม task ที่เมษาอนุมัติ การพัฒนา Cleaning ใช้ `CleaningSystem` เมื่อเมษาหรือทีมจัดเตรียม branch แล้ว ห้ามสร้าง branch หรือ worktree เอง ใช้กติกาการจัดการ Git ใน [AGENTS.md](AGENTS.md)
- ตรวจโฟลเดอร์ คลาส โมดูล interface และ naming ที่มีอยู่ก่อนเลือกไฟล์สำหรับอุปกรณ์ เป้าหมาย สถานะ และความคืบหน้า Cleaning ใช้โครงสร้างเดิมตาม Team Workflow และรออนุมัติหากต้องเพิ่มโฟลเดอร์ ประสาน Codex เรื่องสัญญาร่วมและขออนุญาตเจ้าของก่อนแตะ `Shared/`
- ใช้ fixture ที่รับ `Pawn` และ `Actor` หรือ Blueprint mock ตามสัญญาที่ตกลง ห้าม cast ไปยัง Player Blueprint ของ Codex
- ตกลงผู้ดูแล asset path, pins และ outputs กับเมษาและ Codex ก่อนสร้าง Blueprint Interface ที่อนุมัติแล้ว
- บันทึกผล Compile และสถานะ `IMPLEMENTED - PENDING MANUAL TEST` ใน [STATUS](Docs/AI/STATUS.md) และ [TASKS](Docs/AI/TASKS.md) โดยประสานผู้เขียนก่อนแก้เอกสารร่วม


## ขอบเขตปัจจุบัน

P-01 เป็นงาน Single Player ของ Codex เท่านั้น Cleaning และงาน multiplayer ไม่อยู่ใน P-01 ให้รออนุมัติ task ของตัวเองก่อนเริ่ม

## ใช้ Unreal MCP สำหรับ Cleaning ที่อนุมัติแล้ว

- ใช้ตัวเปิด `ScrubbyBooGame - EngineWire.lnk` ตาม [STATUS](Docs/AI/STATUS.md#unreal-mcp-ที่ตรวจแล้ว) เช็ก `claude mcp list` และเรียก `unreal` ด้วย `manage_level.list_levels` ให้แน่ใจว่าเป็น Editor และแผนที่ของงานก่อนแก้
- ใช้ `search` / `describe` สำหรับ capability ที่ต้องใช้ แล้วทำ Compile, Save และเปิด asset ตาม Implementation Only หาก MCP ไม่พร้อม ให้รายงาน blocker
