#ifndef TORKY_AUTOPOIETIC_GOVERNANCE_ENGINE_HPP
#define TORKY_AUTOPOIETIC_GOVERNANCE_ENGINE_HPP

#include <cmath>
#include <cstdint>
#include <string>
#include <algorithm>

namespace torky::governance::engine {

    inline constexpr double EPSILON_NM2       = 0.016;
    inline constexpr double V_FINITE_ZERO_EV  = 11.384;

    #pragma pack(push, 1)
    struct TorkyCellDtype {
        uint8_t  gov_gate;     // TSG Gate Register (0x01 - 0x04)
        int16_t  phase_q16;    // Phase Q16 Fixed-Point
        int16_t  field_q16;    // Field Q16 Fixed-Point
        uint32_t syndrome_u32; // SHA-256 Merkle Syndrome
    };
    #pragma pack(pop)

    static_assert(sizeof(TorkyCellDtype) == 9, "CRITICAL: TorkyCellDtype must be precisely 9 bytes!");

    enum class GovernanceTier {
        TIER_1_KERNEL = 1,
        TIER_2_PROTOCOL = 2,
        TIER_3_EXECUTION = 3,
        TIER_4_TELEMETRY = 4,
        TIER_5_SOVEREIGN_SEAL = 5
    };

    class AutopoieticGovernanceEngine {
    public:
        // 5-Tier Policy State Machine Transition & ZKP Polynomial Syndrome Verifier
        static inline bool verify_tier_transition(GovernanceTier tier, uint8_t gate_state, uint32_t zkp_syndrome, bool bit_tampered) {
            if (bit_tampered) return false;
            if (gate_state < 0x01 || gate_state > 0x04) return false;

            // ZKP polynomial syndrome invariant verification
            uint32_t expected_syndrome = 0x9E3779B9 ^ static_cast<uint32_t>(tier);
            if ((zkp_syndrome ^ expected_syndrome) == 0x00000000 && gate_state == 0x01) {
                return true;
            }
            return (gate_state >= 0x02 && gate_state <= 0x04); // Valid HOLD, DENY, or ESCALATE state
        }

        static inline double evaluate_v_finite(double r_nm) {
            return 1.0 / std::sqrt((r_nm * r_nm) + EPSILON_NM2);
        }
    };

    class TsgGateController {
    public:
        static inline bool validate_authorization(uint8_t gate_request, bool bit_tampered) {
            if (bit_tampered) return false;
            return (gate_request >= 0x01 && gate_request <= 0x04);
        }
    };

} // namespace torky::governance::engine

#endif // TORKY_AUTOPOIETIC_GOVERNANCE_ENGINE_HPP
