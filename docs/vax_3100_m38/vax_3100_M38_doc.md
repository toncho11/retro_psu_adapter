# DEC VAXstation 3100 Model 38 (KA42-B, BA42-A enclosure)  

## Original PSU and outputs

The BA42-A VAXstation 3100 Model 38 uses the **H7821-00** supply. The H7821 family is also found in other smaller VAXstation/DECstation/MicroVAX systems; the larger BA42-B systems use H7822 variants. A restoration reference identifies the H7821-00 as an approximately **180 W** power supply; verify the nameplate of the specific PSU.The original H7821's **mainboard-side connector has 14 positions; the motherboard connector is female and the mating harness plug is male**.

A person has recently reversed engineered the VAX's PSU: https://forum.vcfed.org/index.php?threads/vaxstation-issues.40373/page-3#post-1515713

### 14-pin motherboard power connector

This is the **electrical pin assignment**, *not* a face-view drawing. Do not assume that the numbering runs left-to-right when looking at the female socket. Pin numbering / face orientation must be verified against the actual PCB, harness and keyed connector. A restoration reference cautions that the illustration in one DEC guide has reversed numbering.

| Pin | Signal | Nominal voltage / meaning | Original wire color |
|---:|---|---|---|
| 1 | `VAX_-12V` | −12.1 V relative to system ground (±5%) | Blue |
| 2 | `GND` | System ground | Black |
| 3 | `GND` | System ground | Black |
| 4 | `GND` | System ground | Black |
| 5 | `VAX_+5V` | +5.1 V (±5%) | Red |
| 6 | `VAX_+5V` | +5.1 V (±5%) | Red |
| 7 | `DCOK` | Power-good: **HIGH = OK**, nominally +3.5 to +5.25 V; LOW = not OK | Brown |
| 8 | `VAX_+5V` | +5.1 V (±5%) | Red |
| 9 | `VAX_+12V` | +12.1 V (±5%) | Orange |
| 10 | `VAX_+5V` | +5.1 V (±5%) | Red |
| 11 | `GND` | System ground | Black |
| 12 | `GND` | System ground | Black |
| 13 | `ISO_-9V` | −9 V measured **relative to pin 14** (±5%) | Yellow |
| 14 | `ISO_9V_RETURN` | Return for the **isolated** −9 V supply; **not automatically system ground** | White |

**Crucial:** DEC instructs measuring pin 13 with the meter's negative lead on **pin 14**, not on a black ground pin. The −9 V pair is galvanically isolated from the ordinary system return at the PSU output.

### Load currents — what is known

The DEC maintenance voltage table above **does not specify H7821 Model 38 per-rail output current ratings**. The following numbers are not verified.

| Rail | Original nominal voltage | Current information |
|---|---|---|
| +5 V | +5.1 V | ~15–18 A is an estimate, not a confirmed Model 38 continuous rating. |
| +12 V | +12.1 V | ~4.5–6 A is an estimate, not a confirmed Model 38 rating; additional drive loads may exist on separate harnesses. Mainly used by the disk drives, floppy and other peripherals. |
| −12 V | −12.1 V | Between 0.1A and 0.8A, but most likely 0.3A. Probably used only for the serial port, which consumes around 0.1A. |
| Isolated −9 V | −9 V across pins 13–14 | Precise Model 38 max current not established. Must be provided from an isolated source on pins 13 and 14. Used only for networking. Not critical for operating the Vax. |
| DCOK | Logic signal | Power good signal |

WARNING: For an ATX splitter, use only one with sufficiently tick tracks that can suppoirt the above high currents.

The **180 W** figure does not mean a single rail can draw the full 180 W. Also 5V 18A are separated in several 5V rails and several GND rails. 

## How to reproduce the isolated −9 V output

The isolated supply has been **suggested** as supplying the built-in ThinWire (10BASE2) Ethernet interface. Omitting the the -9V does not prevent the Vax from booting. A conventional ATX PSU typically has a common-ground −12 V output; **it does not directly provide the isolated −9 V pair** used on the DEC connector.

We can use an appropriately rated **isolated DC/DC module** with a regulated 9 V output and suitable input range, isolation rating, efficiency, ripple and startup behavior. To create the DEC negative rail with the correct isolated polarity:

```text
      ATX +5 V or +12 V  ----> [ ISOLATED 9 V DC/DC ]
      ATX GND            ----> [   primary input    ]

                                  secondary +9V ----> VAX pin 14 (ISO_9V_RETURN)
                                  secondary  0V ----> VAX pin 13 (ISO_-9V)

                   Voltage measured: V(pin13) - V(pin14) = -9 V
```

**Do not tie pin 14 to ATX GND by default.** The secondary must remain floating unless actual DEC motherboard wiring and the chosen converter's datasheet demonstrate that a different arrangement is correct. A simple non-isolated charge pump / negative voltage regulator does **not** reproduce galvanic isolation. Do not connect ATX −12 V directly to pin 13 as a substitute.
