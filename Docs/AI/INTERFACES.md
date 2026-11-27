# สัญญา Blueprint ระหว่าง Player และ Cleaning

P-01 Single Player ไม่มี interface ที่ต้องสร้างหรือใช้งาน สัญญาด้านล่างเป็นร่างสำหรับงานที่ต้องอนุมัติแยก รวมถึงส่วน multiplayer

ร่างสำหรับ PC-01 · รอเมษาและผู้รับผิดชอบทั้งสองระบบตกลงก่อนสร้าง asset งาน P-01 ทำแยกได้ WorkPlan หน้า 9 กำหนดอุปกรณ์และการเก็บ progress เมื่อพักถู ส่วนปุ่ม ระยะ จังหวะการประมวลผล และสูตร progress ให้ยืนยันใน task ที่เกี่ยวข้อง

## ส่งต่อการเชื่อมแบบผู้เล่นเดียว

สถานะ **ยังไม่ได้เชื่อม Player จริงกับ Cleaning** · Player อยู่บน `PlayerSystem` ที่ `C:/Users/masap/GameLab` และ Cleaning อยู่บน `CleaningSystem` ที่ `C:/Users/masap/GameLab-Cleaning` ให้เมษากับ Git Manager จัดการส่งต่อ revision ก่อนประกอบร่วม

- **Player** `/Game/ScrubbyBoo/Blueprints/Player/BP_PlayerFoundation` เป็นเจ้าของการจับ/ปล่อยผ่าน `TryGrab`, `ReleaseGrab` และ `UpdateGrabTargets` `GetLeftGrabContact` และ `GetRightGrabContact` คืน `HasContact` และ `WorldContact` ให้ `/Game/ScrubbyBoo/Blueprints/Player/ABP_PlayerGrabIK` ใช้ในงานแสดงท่าจับเท่านั้น จุด IK ไม่ใช่ตัวแทนสถานะผู้ถือหรือเป้าหมายทำความสะอาด
- **Cleaning** `/Game/ScrubbyBoo/Blueprints/Cleaning/BP_CleaningTool` เป็นฐานของ `BP_Mop`, `BP_CobwebBroom` และ `BP_GarbageBag` `BP_Stain`, `BP_Cobweb` และ `BP_Garbage` เป็นเป้าหมายต้นแบบ ใช้ `/Game/ScrubbyBoo/Maps/Lvl_Test_Cleaning` กับ `BP_CleaningTestPawn` และ `BP_CleaningTestGameMode` ในโฟลเดอร์ Cleaning เพื่อทดสอบแยก
- **หยิบ/ปล่อย** ใช้ `TryGrab` ที่สำเร็จและเส้นทางปล่อยจาก `ReleaseGrab` เป็นจุดต่อ หลังอัปเดตสถานะมือให้ตรวจ held actor เดิมและปัจจุบัน เมื่อร่าง `BPI_PlayerInteractable` ได้รับอนุมัติ ให้ส่ง `NotifyHolderChanged(PreviousHolder, NewHolder)` ไปยัง held actor เมื่อมีมือแรกจับหรือมือสุดท้ายปล่อย สองมือที่ใช้ Handle ร่วมกันยังมีผู้ถือคนเดียว ครอบคลุมการหลุดและการทำลายพร็อพด้วย ถ้า actor ถูกทำลายแล้ว ให้ล้างสถานะโดยไม่ส่ง message ไปยัง reference ที่หมดอายุ
- **ตัวรับใน Cleaning** ให้ `BP_CleaningTool` รับสถานะผู้ถือผ่าน `Pawn`/`Actor` แล้วปรับ `Holder` และ `bCleaningActive` ให้สอดคล้องกับสถานะใหม่ ต้นแบบเดิมใช้ `NotifyGrabbed`/`NotifyReleased` แบบต่อมือและ `HeldHandCount` ให้ adapter ใช้การแจ้งแบบมือแรก/มือสุดท้ายเป็นแหล่งเดียวต่อการเชื่อม ม็อบและไม้กวาดตรวจ overlap ของ `HeadCleanZone` ส่วนถุงให้ `TryInteract(TargetActor, …)` บนของที่ถือเรียก `CollectGarbage(Target)` โดยใช้กติกาของถุง ปุ่ม E ยังอยู่เฉพาะใน fixture ของ Cleaning
- BPI ที่เสนอ `/Game/ScrubbyBoo/Blueprints/BPI_PlayerInteractable` ยังไม่มี asset และยังต้องอนุมัติ PC-01 งานส่งต่อรอบนี้คงขอบเขตผู้เล่นเดียว

## Asset และความรับผิดชอบ

- เสนอ `Content/ScrubbyBoo/Blueprints/BPI_PlayerInteractable.uasset` (`/Game/ScrubbyBoo/Blueprints/BPI_PlayerInteractable`) ในโฟลเดอร์ Content ที่มีอยู่ ผู้ดูแลสัญญาที่เสนอคือเมษา ผู้สร้าง asset คนเดียวที่เสนอคือ Codex หลังเมษาอนุมัติและ Claude Code ตรวจ pins ฝั่ง Cleaning ให้ทั้งสอง branch ใช้ asset revision เดียวกันหลังทีมจัดการส่งต่อ
- **Player · Codex** รับ input เลือกเป้าหมาย และเป็นเจ้าของสถานะถือ/ปล่อยบน server ส่ง Blueprint Interface Message ไปยัง actor ผ่านชนิด Engine ทั่วไป กติกาอุปกรณ์และ Cleaning อยู่ฝั่งผู้รับ
- **Cleaning · Claude Code** รับ message ในอุปกรณ์และเป้าหมาย ตรวจว่าอุปกรณ์ทำงานกับเป้าหมายได้ และเป็นเจ้าของ progress ฝั่ง server ใช้ `Pawn Object Reference` และ `Actor Object Reference` ในสัญญา โดยไม่ cast ไปยัง `BP_PlayerFoundation` หรือ bind delegate ของ Player

## Pins ที่เสนอ

| Function | Receiver และ input | Output และความหมาย |
| --- | --- | --- |
| `TryInteract` | Actor ที่ implement BPI รับ `InstigatorPawn` (Pawn), `TargetActor` (Actor, ว่างได้เฉพาะกรณีที่อนุมัติ), `HeldActor` (Actor, ว่างเมื่อไม่ได้ถือ), `HitComponent` (Primitive Component, optional), `HitLocation` (Vector, มีความหมายเมื่อมี hit ที่ server ตรวจแล้ว) | `Accepted` (Boolean) บอกว่าผู้รับยอมรับการใช้งานหรือคำขอเริ่มใช้งาน ใช้ผลบน server |
| `EndUse` | ผู้รับเดิมของ `TryInteract` รับ `InstigatorPawn` (Pawn) เมื่อปล่อย input หรือการใช้หมดสิทธิ์ | ไม่มี output ยุติ active use ของ pawn ได้อย่างปลอดภัยแม้ถูกเรียกซ้ำ เก็บ progress บนเป้าหมายตาม WorkPlan |
| `NotifyHolderChanged` | held actor ที่ implement BPI รับ `PreviousHolder` และ `NewHolder` (Pawn ทั้งคู่ ว่างได้) หลัง Player เปลี่ยนสถานะถือบน server | ไม่มี output เครื่องมือใช้ยุติ active use ของผู้ถือเดิมและเก็บบริบทผู้ถือใหม่ตามความจำเป็น |

- Player ใช้ `TryInteract (Message)` ที่ actor เป้าหมายสำหรับ interact กับฉาก หรือที่ held actor เมื่อขอใช้ของที่ถือ โดยส่งเป้าหมายที่ตรวจแล้วให้ held actor กติกาฝั่ง Cleaning เลือกว่าต้องคุยกับ target อย่างไร และอาจใช้ message สัญญาเดียวกันไปยัง target เมื่ออุปกรณ์ถูกต้อง ให้ target ประมวลผลงานตรงนั้นและหลีกเลี่ยงการเรียกกลับวน งานแบบต่อเนื่องให้ผู้รับ `TryInteract` ประสาน `EndUse` ไปยัง target ที่ตนเริ่มงานไว้ตามจำเป็น
- สำหรับงานต่อเนื่อง ผล Accepted หมายถึงเริ่มหรือยอมรับ active use เท่านั้น Cleaning คำนวณงานบน server ตรวจ holder/target และเงื่อนไขซ้ำก่อนเพิ่ม progress สิ้นสุดเมื่อได้ `EndUse`, เปลี่ยนผู้ถือ หรือ reference/เงื่อนไขหมดอายุ การส่งซ้ำหรือการใช้งานพร้อมกันต้องไม่เพิ่มเครดิตเสร็จงานซ้ำ

## Multiplayer และการทดสอบ

- Blueprint Interface Message เป็น local dispatch ในเครื่องที่เรียก ให้ input ของ remote client ส่งเจตนาผ่าน Custom Event แบบ **Run on Server** บน Player pawn/controller ที่ client เป็นเจ้าของ และให้ Host เข้าทาง validation เดียวกัน Server ยืนยัน possession, ระยะ, แนวบัง, target และ held actor จาก state จริง ก่อนส่ง BPI message
- Player ถือ state `HeldActor` ส่วน Cleaning ถือ state ของ target และความคืบหน้ารวม ใช้ **Replicated** / **RepNotify** ตามผู้สังเกตที่ต้องเห็น ให้ late join อ่าน snapshot และให้ความพร้อมออกอิงงานบังคับจริง ผู้รับข้อมูล progress รวมในอนาคตคือเงื่อนไขออกด่านและ UI ตามขอบเขตที่ได้รับอนุมัติ
- ทดสอบ Player ด้วย actor fixture ทั่วไป และ Cleaning ด้วย fixture ฝั่ง server ที่ใช้ `Pawn`/`Actor` ก่อนทดสอบร่วม กระตุ้นจาก Host และ Client ทดสอบ wrong tool, target ที่ถูกบัง, duplicate request, พัก/ถูต่อ, tool หลุดหรือถูกทำลาย และ late join ไม่ต้องใช้ Ghost AI หรือระบบอื่นที่สงวนไว้

## ผลจากต้นแบบ Cleaning Tools · 27 พฤศจิกายน 2026 (Claude Code)

**ข้อเสนอ: ใช้ `BPI_PlayerInteractable` ของ Codex เป็น interface เดียว และถอน `BPI_HeldItemUse`** ต้นแบบแสดงว่า Cleaning ต้องการจาก Player เพียงสองอย่างด้านล่าง ซึ่ง pins ของ Codex รองรับแล้ว ยังไม่ได้สร้าง interface asset ใด ๆ จนกว่าเมษาอนุมัติและ Codex รีวิว

| หน้าที่ | เจ้าของ | วิธีในต้นแบบ | จับคู่กับ `BPI_PlayerInteractable` |
| --- | --- | --- | --- |
| จับ/ปล่อยวัตถุ | Player (Codex) | Physics Handle มือซ้าย/ขวาใน `BP_PlayerFoundation` | ไม่ต้องแก้ |
| รู้ว่าอุปกรณ์ถูกถืออยู่ | Player แจ้ง → Cleaning ตัดสิน | ชั่วคราว: เรียก `NotifyGrabbed(NewHolder)` / `NotifyReleased(OldHolder)` บน `BP_CleaningTool` ต่อมือหนึ่งครั้ง | `NotifyHolderChanged(PreviousHolder, NewHolder)` บน held actor เรียกเมื่อมือแรกจับและมือสุดท้ายปล่อย |
| Interact กับขยะ | Player ส่ง → ถุงตัดสิน | ชั่วคราว: test pawn กด E แล้วเรียก `CollectGarbage(Target)` บน `BP_GarbageBag` | `TryInteract` บน held actor โดยส่ง `TargetActor` ที่ trace ได้ ถุงตอบ `Accepted` |
| ทำความสะอาดด้วยการสัมผัส (ม็อบ/ไม้กวาด) | Cleaning | `HeadCleanZone` overlap ของหัวอุปกรณ์ ไม่ต้องกดปุ่ม | ไม่ใช้ interface |

- Cleaning ไม่ cast ไปยัง Player ตรวจเฉพาะ `Actor` ผู้ถือ กติกา "ถืออุปกรณ์ทำความสะอาดได้ครั้งละหนึ่งชิ้นที่ทำงาน" ตัดสินในฝั่ง Cleaning โดยเทียบผู้ถือเดียวกัน
- จุดที่ Codex ต้องตัดสิน: เมื่อเชื่อมจริง `BP_PlayerFoundation` ต้องเรียก message หยิบ/ปล่อยบนอุปกรณ์ทุกกรณีที่ของออกจากมือ ปัจจุบัน grab ของ Codex ยังไม่แจ้งอุปกรณ์ จึงยังไม่ได้ทดสอบร่วม

## `BPI_HeldItemUse` · สถานะ PROPOSED (เสนอให้ถอน ดูหัวข้อด้านบน)

เสนอโดย Claude Code วันที่ 27 พฤศจิกายน 2026 · **ยังไม่อนุมัติ** รอ Codex รีวิวและเมษาอนุมัติ ห้ามสร้าง asset จนกว่าสถานะจะเปลี่ยนเป็น APPROVED

**ข้อควรตัดสินก่อน:** `BPI_PlayerInteractable` ของ Codex ด้านบนครอบคลุมหน้าที่เดียวกันแล้ว (`TryInteract` บน held actor ≈ เริ่มใช้, `EndUse` ≈ หยุดใช้, `NotifyHolderChanged` ≈ หยิบ/ปล่อย) ตามกติกาห้ามสร้าง interface ซ้ำ Claude Code เสนอให้เลือก **interface เดียว** ข้อเสนอนี้จึงเป็นทางเลือกเปรียบเทียบ หากเลือก `BPI_PlayerInteractable` ให้ยกเลิก `BPI_HeldItemUse` และนำข้อเรียกร้องฝั่ง Cleaning ในตาราง "ส่วนต่าง" ไปเพิ่มใน interface นั้นแทน

### ความเป็นเจ้าของ

- ผู้ดูแลสัญญา: เมษา · ผู้สร้าง asset คนเดียว: ตกลงตอนอนุมัติ (เสนอ Codex ตามแนวทางเดิมด้านบน) · ผู้ตรวจ pins: Claude Code (receiver) และ Codex (caller)
- Path ที่เสนอ: ใช้ path เดียวกับที่ตกลงสำหรับ `BPI_PlayerInteractable` เพื่อให้มี interface ร่วมจุดเดียว ทั้งสอง branch ใช้ revision เดียวกัน
- ผู้เรียก (caller): Player ของ Codex หรือ test pawn ของ Cleaning · ผู้รับ (receiver): อุปกรณ์ทำความสะอาดที่ถูกถือ (`BP_CleaningTool_*`)

### Functions

| Function | Input | Output | Caller ต้องทำ | Receiver ต้องทำ |
| --- | --- | --- | --- | --- |
| `HeldItem_OnPickedUp` | `Holder` (Pawn) | – | เรียกบน server หลังยืนยันการหยิบแล้ว | บันทึกผู้ถือ ไม่ cast หา Player |
| `HeldItem_OnDropped` | `Holder` (Pawn) | – | เรียกบน server ทุกกรณีที่ของออกจากมือ: ปล่อยเอง หลุดเพราะชน ถูกแย่ง เลิก possess หรือ disconnect | หยุดการใช้ที่ค้าง ล้างผู้ถือ progress ที่สะสมบนเป้าหมายต้องคงอยู่ |
| `HeldItem_UseStart` | `User` (Pawn) | `Accepted` (Boolean) | เรียกบน server เมื่อกดใช้ หลัง Run on Server ตรวจ possession แล้ว | ตรวจว่า User เป็นผู้ถือ หาเป้าหมายจากมุมมองของ User ฝั่ง server ประมวลผลงานต่อเนื่องด้วย timer ฝั่ง server |
| `HeldItem_UseStop` | `User` (Pawn) | – | เรียกบน server เมื่อปล่อยปุ่มใช้ | หยุด timer เรียกซ้ำได้อย่างปลอดภัย |
| `HeldItem_GetUseRange` | – | `Range` (Float, cm) | ใช้ประกอบการเล็ง/highlight เท่านั้น | คืนระยะเอื้อมของอุปกรณ์ ค่าที่ server ใช้ตรวจจริงอยู่ฝั่ง receiver |

### Dependency และพฤติกรรมที่คาดหวัง

- Caller ต้องมี state ผู้ถือบน server และ Run on Server event บน pawn/controller ที่ client เป็นเจ้าของ ส่วน receiver ใช้เพียง `Pawn` และ `Get Actor Eyes View Point` ของ Engine
- ทุก function เรียกบน server เท่านั้น เพราะ BPI message ทำงานในเครื่องที่เรียก Host เข้าเส้นทางเดียวกับ Client
- การเรียกซ้ำหรือผู้เล่นหลายคนใช้พร้อมกันต้องไม่ให้เครดิตงานเสร็จซ้ำ ข้อนี้ receiver รับผิดชอบ
- อ้าง WorkPlan หน้า 9 สำหรับถุงขยะ ไม้กวาด ม็อบ และการเก็บ progress เมื่อพักถู ส่วนหน้า 9–10 ระบุว่าการชน/ล้มและผีทำให้ของหลุดมือ caller จึงต้องเรียก `HeldItem_OnDropped` ทุกกรณี

### ส่วนต่างจาก `BPI_PlayerInteractable`

| ประเด็น | `BPI_PlayerInteractable` | `BPI_HeldItemUse` | ผลต่อ Cleaning |
| --- | --- | --- | --- |
| ใครหาเป้าหมาย | Player trace แล้วส่ง `TargetActor` ที่ตรวจแล้ว | อุปกรณ์ trace เองบน server ทุกครั้งที่ประมวลผล | งานต่อเนื่องต้องรู้เป้าหมายปัจจุบันระหว่างถู ถ้าใช้แบบ Codex ต้องตกลงว่า Player ส่งเป้าหมายใหม่เมื่อเล็งเปลี่ยนหรือไม่ |
| ระยะเอื้อม | ไม่มี | `HeldItem_GetUseRange` | ไม้กวาดต้องเอื้อมถึงเพดาน (หน้า 9) Player ต้องรู้ระยะของอุปกรณ์ |
| ขอบเขต | ทั้งของในฉากและของที่ถือ | เฉพาะของที่ถือ | แบบ Codex ใช้ interface เดียวทั้งเกม ลดจำนวน asset |

### เรื่องที่ยังไม่ตัดสิน

1. เลือก interface เดียว: `BPI_PlayerInteractable` (ของ Codex) หรือ `BPI_HeldItemUse`
2. ใครเป็นผู้หาเป้าหมายระหว่างงานต่อเนื่อง และความถี่ในการตรวจ
3. วิธีส่งระยะเอื้อมของอุปกรณ์ให้ Player
4. Path, ผู้สร้าง asset และ revision ที่ทั้งสอง branch ใช้
5. กลไกที่ WorkPlan ยังไม่สรุปและสัญญานี้ไม่กำหนด: ปุ่มจริง, วิธีทิ้งถุง, ความจุถุง, สูตร %, การคืนอุปกรณ์ที่เอื้อมไม่ถึง
