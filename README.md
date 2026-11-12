# What is it?
A retro atx power adapter for retro computers that focuses on the POWER GOOD (PG) signal (also called DCOK), voltage protection and diagnostic for old computers such as DEC VAX 3100 M38 and SGI Indy. This project can be used in hte case your original power supply unit (PSU) has failed or you prefer not to use it.

# Models
| Model | Status | Description |
|---|---|---|
| [Model_AA](https://github.com/toncho11/retro_psu_adapter/tree/main/Model_AA) | Available | Handles PG and delays. Provides light voltage protection and diagnostic. Does not handle the high current rails of the PSU, this is done by the DS1188. |
| Model_BB | Planned | Same as Model_AA, but now it handles the high current rails from the ATX PSU. Adds fan connectors. Should disable PSU if a fan has stopped for more than 1 minute. |

# Disclaimer

**⚠️ WARNING: ELECTRICAL SHOCK, FIRE, AND EQUIPMENT DAMAGE HAZARD**

This project is provided **"AS IS", without any warranty or guarantee of safety, reliability, or fitness for a particular purpose.** To the extent permitted by applicable law, the author and individual contributors disclaim liability for personal injury, death, property damage, or equipment damage resulting from the assembly, modification, installation, or use of these PSU adapters.

**You build and use these adapters entirely at your own risk!**

Electricity is dangerous and must never be taken lightly. Incorrect wiring, inadequate grounding, short circuits, or improper handling can cause serious injury, electric shock, fire, or permanent damage to your computer and its components.

Always observe the following safety precautions:

- **Never leave exposed or dangling wires.** Ensure all connections are properly insulated and secured.
- **Never touch a PCB or exposed electrical contacts while the system is powered on.**
- **Always disconnect the power supply from the mains before installing, modifying, or inspecting the adapter.** Allow time for internal capacitors to discharge.
- **Ensure the PSU is properly grounded** through its protective earth connection.
- **Ensure the computer's metal chassis is properly connected to protective earth.**
- **Verify all connections, pinouts, polarities, and voltage levels before powering on the computer.**
- **Never bypass protective grounding or other safety mechanisms.**

**A particularly dangerous situation can occur when an exposed conductive surface becomes energized due to a fault.** You may initially feel nothing if there is no effective current path through your body. However, touching another conductive or grounded surface with your hand, leg, or another part of your body can complete that path, allowing potentially lethal current to flow. **The absence of an electric shock does not mean a surface is safe to touch!**

These adapters are experimental hardware intended for users with appropriate electronics knowledge and experience. They are **NOT certified safety devices** and must **NOT** be assumed to provide protection against electrical shock, fire, or equipment damage.

**If you are unsure about any aspect of electrical safety, do not proceed without assistance from a qualified professional.**
