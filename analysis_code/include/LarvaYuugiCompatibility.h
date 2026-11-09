#ifndef LARVA_YUUGI_COMPATIBILITY_H
#define LARVA_YUUGI_COMPATIBILITY_H

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "Loader.h"

// Temporary compatibility for CTRL samples made with Larva (MC) and Yuugi
// (data). Remove these calls once all inputs use the same production schema.
namespace LarvaYuugiCompatibility {

class NormalizeTriggers final : public Module::Module {
private:
    std::vector<std::size_t> legacy_trigger_indices;
    std::size_t replaced_nan_count = 0;

public:
    NormalizeTriggers(std::vector<std::string>& names,
                      const std::vector<std::string>& types) {
        if (names.size() != types.size()) {
            throw std::runtime_error("[LarvaYuugiCompatibility] Inconsistent branch names/types.");
        }

        std::vector<std::string> bits = {"fff", "ffo", "fyo", "ffy", "stt", "hie"};
        for (int i = 0; i <= 13; ++i) bits.push_back("lml" + std::to_string(i));

        for (const std::string& family : {"L1FTDL", "L1PSNM"}) {
            for (const std::string& bit : bits) {
                const std::string legacy = family + "__bo" + bit + "__bc";
                const std::string canonical = "ifNANgiveX__bo" + legacy + "__cm__mi1__bc";
                const auto old_it = std::find(names.begin(), names.end(), legacy);
                const auto new_it = std::find(names.begin(), names.end(), canonical);

                if (old_it != names.end() && new_it != names.end()) {
                    throw std::runtime_error("[LarvaYuugiCompatibility] Both trigger forms exist: " + legacy);
                }
                if (old_it == names.end() && new_it == names.end()) {
                    throw std::runtime_error("[LarvaYuugiCompatibility] Missing trigger: " + canonical);
                }

                const auto source_it = old_it != names.end() ? old_it : new_it;
                const std::size_t index = static_cast<std::size_t>(source_it - names.begin());
                if (types.at(index) != "Double_t") {
                    throw std::runtime_error("[LarvaYuugiCompatibility] Expected Double_t for " + *source_it);
                }

                if (old_it != names.end()) {
                    // Rename in place: appending aliases would change the branch order
                    // relative to Yuugi, which the downstream Loader checks strictly.
                    names.at(index) = canonical;
                    legacy_trigger_indices.push_back(index);
                }
            }
        }
    }

    void Start() override {
        std::printf("[LarvaYuugiCompatibility] Normalizing %zu Larva trigger branches.\n",
                    legacy_trigger_indices.size());
    }

    int Process(std::deque<Data>* data) override {
        for (Data& event : *data) {
            for (std::size_t index : legacy_trigger_indices) {
                double& value = std::get<double>(event.variable.at(index));
                if (std::isnan(value)) {
                    value = -1.0;
                    ++replaced_nan_count;
                }
            }
        }
        return 1;
    }

    void End() override {
        std::printf("[LarvaYuugiCompatibility] Replaced %zu trigger NaNs with -1.\n",
                    replaced_nan_count);
    }

    std::optional<std::set<std::string>> RequiredVariables() const override {
        // Keep all input columns, including the legacy names read by LoadWithCut.
        return std::nullopt;
    }
};

} // namespace LarvaYuugiCompatibility

// Call immediately after LoadWithCut, before weights, selections and outputs.
// Input files within one LoadWithCut call must already share a schema.
inline void ApplyTemporaryLarvaYuugiCompatibility(Loader& loader) {
    const std::size_t original_size = loader.Getvariable_names_address()->size();
    loader.RemoveVariable({
        "extraInfo__bon_vpho_eID5__bc",
        "extraInfo__bon_vpho_eID9__bc",
        "extraInfo__bon_vpho_eID95__bc",
        "extraInfo__bon_vpho_brem_eID5__bc",
        "extraInfo__bon_vpho_brem_eID9__bc",
        "extraInfo__bon_vpho_brem_eID95__bc",
        "extraInfo__bon_vpho_muID5__bc",
        "extraInfo__bon_vpho_muID9__bc",
        "extraInfo__bon_vpho_muID95__bc",
        "extraInfo__boMCALPFlag__bc",
        "extraInfo__boMCALPinvMOneTwo__bc",
        "extraInfo__boMCALPinvMOneThree__bc",
        "extraInfo__boMCALPinvMTwoThree__bc",
        "extraInfo__boMCPromptFlag__bc",
        "extraInfo__boMCPromptinvMOneTwo__bc",
        "extraInfo__boMCPromptinvMOneThree__bc",
        "extraInfo__boMCPromptinvMTwoThree__bc"
    });
    std::printf("[LarvaYuugiCompatibility] Removed %zu Yuugi-only branches.\n",
                original_size - loader.Getvariable_names_address()->size());
    loader.InsertCustomizedModule(new LarvaYuugiCompatibility::NormalizeTriggers(
        *loader.Getvariable_names_address(), *loader.VariableTypes_address()));
}

#endif
