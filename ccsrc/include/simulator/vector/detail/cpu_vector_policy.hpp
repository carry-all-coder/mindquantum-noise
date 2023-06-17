//   Copyright 2022 <Huawei Technologies Co., Ltd>
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#ifndef INCLUDE_VECTOR_DETAIL_CPU_VECTOR_POLICY_HPP
#define INCLUDE_VECTOR_DETAIL_CPU_VECTOR_POLICY_HPP

#include <complex>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>

#include "config/openmp.hpp"

#include "core/mq_base_types.hpp"
#include "core/sparse/csrhdmatrix.hpp"
#include "core/utils.hpp"
#include "math/tensor/traits.hpp"
#include "simulator/types.hpp"

namespace mindquantum::sim::vector::detail {
struct CPUVectorPolicyAvxFloat;
struct CPUVectorPolicyAvxDouble;

template <typename derived_, typename calc_type_>
struct CPUVectorPolicyBase {
    using derived = derived_;
    using calc_type = calc_type_;
    using qs_data_t = std::complex<calc_type>;
    using qs_data_p_t = qs_data_t*;
    using py_qs_data_t = std::complex<calc_type>;
    static constexpr tensor::TDtype dtype = tensor::to_dtype_v<py_qs_data_t>;
    static constexpr index_t DimTh = 1UL << 13;

    static constexpr qs_data_t IMAGE_MI = {0, -1};
    static constexpr qs_data_t IMAGE_I = {0, 1};

    // basic
    // ========================================================================================================

    static qs_data_p_t InitState(index_t dim, bool zero_state = true);
    static void Reset(qs_data_p_t qs, index_t dim);
    static void FreeState(qs_data_p_t qs);
    static void Display(qs_data_p_t qs, qbit_t n_qubits, qbit_t q_limit = 10);
    static void SetToZeroExcept(qs_data_p_t qs, index_t ctrl_mask, index_t dim);
    template <index_t mask, index_t condi, class binary_op>
    static void ConditionalBinary(qs_data_p_t src, qs_data_p_t des, qs_data_t succ_coeff, qs_data_t fail_coeff,
                                  index_t dim, const binary_op& op);
    template <class binary_op>
    static void ConditionalBinary(qs_data_p_t src, qs_data_p_t des, index_t mask, index_t condi, qs_data_t succ_coeff,
                                  qs_data_t fail_coeff, index_t dim, const binary_op& op);
    static void ConditionalAdd(qs_data_p_t src, qs_data_p_t des, index_t mask, index_t condi, qs_data_t succ_coeff,
                               qs_data_t fail_coeff, index_t dim);
    static void ConditionalMinus(qs_data_p_t src, qs_data_p_t des, index_t mask, index_t condi, qs_data_t succ_coeff,
                                 qs_data_t fail_coeff, index_t dim);
    static void ConditionalMul(qs_data_p_t src, qs_data_p_t des, index_t mask, index_t condi, qs_data_t succ_coeff,
                               qs_data_t fail_coeff, index_t dim);
    static void ConditionalDiv(qs_data_p_t src, qs_data_p_t des, index_t mask, index_t condi, qs_data_t succ_coeff,
                               qs_data_t fail_coeff, index_t dim);
    static void QSMulValue(qs_data_p_t src, qs_data_p_t des, qs_data_t value, index_t dim);
    static qs_data_t ConditionalCollect(qs_data_p_t qs, index_t mask, index_t condi, bool abs, index_t dim);
    static VT<py_qs_data_t> GetQS(qs_data_p_t qs, index_t dim);
    static void SetQS(qs_data_p_t qs, const VT<qs_data_t>& qs_out, index_t dim);
    static qs_data_p_t ApplyTerms(qs_data_p_t qs, const std::vector<PauliTerm<calc_type>>& ham, index_t dim);
    static qs_data_p_t Copy(qs_data_p_t qs, index_t dim);
    template <index_t mask, index_t condi>
    static py_qs_data_t ConditionVdot(qs_data_p_t bra, qs_data_p_t ket, index_t dim);
    static py_qs_data_t OneStateVdot(qs_data_p_t bra, qs_data_p_t ket, qbit_t obj_qubit, index_t dim);
    static py_qs_data_t ZeroStateVdot(qs_data_p_t bra, qs_data_p_t ket, qbit_t obj_qubit, index_t dim);
    static py_qs_data_t Vdot(qs_data_p_t bra, qs_data_p_t ket, index_t dim);
    static qs_data_p_t CsrDotVec(const std::shared_ptr<sparse::CsrHdMatrix<calc_type>>& a, qs_data_p_t vec,
                                 index_t dim);
    static qs_data_p_t CsrDotVec(const std::shared_ptr<sparse::CsrHdMatrix<calc_type>>& a,
                                 const std::shared_ptr<sparse::CsrHdMatrix<calc_type>>& b, qs_data_p_t vec,
                                 index_t dim);
    // X like operator
    // ========================================================================================================

    static void ApplyXLike(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, qs_data_t v1, qs_data_t v2,
                           index_t dim);
    static void ApplyX(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplyY(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);

    // Z like operator
    // ========================================================================================================

    static void ApplyZLike(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, qs_data_t val, index_t dim);
    static void ApplyZ(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    // The crazy code spell check in CI do not allow apply s to name following API, even I set the filter file.
    static void ApplySGate(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplyT(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplySdag(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplyTdag(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplyPS(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                        bool diff = false);

    // Single qubit operator
    // ========================================================================================================

    static void ApplySingleQubitMatrix(qs_data_p_t src, qs_data_p_t des, qbit_t obj_qubit, const qbits_t& ctrls,
                                       const std::vector<std::vector<py_qs_data_t>>& m, index_t dim);
    static void ApplyTwoQubitsMatrix(qs_data_p_t src, qs_data_p_t des, const qbits_t& objs, const qbits_t& ctrls,
                                     const std::vector<std::vector<py_qs_data_t>>& m, index_t dim);
    static void ApplyNQubitsMatrix(qs_data_p_t src, qs_data_p_t des, const qbits_t& objs, const qbits_t& ctrls,
                                   const std::vector<std::vector<py_qs_data_t>>& m, index_t dim);
    static void ApplyMatrixGate(qs_data_p_t src, qs_data_p_t des, const qbits_t& objs, const qbits_t& ctrls,
                                const std::vector<std::vector<py_qs_data_t>>& m, index_t dim);
    static void ApplyH(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplyGP(qs_data_p_t qs, qbit_t obj_qubit, const qbits_t& ctrls, calc_type val, index_t dim,
                        bool diff = false);
    static void ApplyRX(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                        bool diff = false);
    static void ApplyRY(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                        bool diff = false);
    static void ApplyRZ(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                        bool diff = false);

    static void ApplySWAP(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, index_t dim);
    static void ApplyISWAP(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, bool daggered, index_t dim);
    static void ApplyRxx(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                         bool diff = false);
    static void ApplyRyy(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                         bool diff = false);
    static void ApplyRzz(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                         bool diff = false);
    static void ApplyRxy(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                         bool diff = false);
    static void ApplyRxz(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                         bool diff = false);
    static void ApplyRyz(qs_data_p_t qs, const qbits_t& objs, const qbits_t& ctrls, calc_type val, index_t dim,
                         bool diff = false);

    // gate_expectation
    // ========================================================================================================
    static qs_data_t ExpectDiffSingleQubitMatrix(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs,
                                                 const qbits_t& ctrls, const VVT<py_qs_data_t>& m, index_t dim);
    static qs_data_t ExpectDiffTwoQubitsMatrix(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs,
                                               const qbits_t& ctrls, const VVT<py_qs_data_t>& m, index_t dim);
    static qs_data_t ExpectDiffNQubitsMatrix(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs,
                                             const qbits_t& ctrls, const VVT<py_qs_data_t>& m, index_t dim);
    static qs_data_t ExpectDiffMatrixGate(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                          const VVT<py_qs_data_t>& m, index_t dim);
    static qs_data_t ExpectDiffRX(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                  calc_type val, index_t dim);
    static qs_data_t ExpectDiffRY(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                  calc_type val, index_t dim);
    static qs_data_t ExpectDiffRZ(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                  calc_type val, index_t dim);
    static qs_data_t ExpectDiffRxx(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                   calc_type val, index_t dim);
    static qs_data_t ExpectDiffRyy(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                   calc_type val, index_t dim);
    static qs_data_t ExpectDiffRzz(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                   calc_type val, index_t dim);
    static qs_data_t ExpectDiffRxy(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                   calc_type val, index_t dim);
    static qs_data_t ExpectDiffRxz(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                   calc_type val, index_t dim);
    static qs_data_t ExpectDiffRyz(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                   calc_type val, index_t dim);
    static qs_data_t ExpectDiffPS(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                  calc_type val, index_t dim);
    static qs_data_t ExpectDiffGP(qs_data_p_t bra, qs_data_p_t ket, const qbits_t& objs, const qbits_t& ctrls,
                                  calc_type val, index_t dim);
    static calc_type GroundStateOfZZs(const std::map<index_t, calc_type>& masks_value, qbit_t n_qubits);
};

template <typename policy_src, typename policy_des>
struct CastTo {
    static constexpr tensor::TDtype src_dtype = policy_src::dtype;
    static constexpr tensor::TDtype des_dtype = policy_des::dtype;
    static typename policy_des::qs_data_p_t cast(typename policy_src::qs_data_p_t qs, size_t dim) {
        if constexpr (std::is_same_v<policy_src, policy_des>) {
            return policy_des::Copy(qs, dim);
        } else {
            auto des = policy_des::InitState(dim, false);
            THRESHOLD_OMP_FOR(
                dim, policy_des::DimTh, for (omp::idx_t i = 0; i < dim; i++) {
                    des[i] = typename policy_des::qs_data_t{std::real(qs[i]), std::imag(qs[i])};
                })
            return des;
        }
    }
};
}  // namespace mindquantum::sim::vector::detail
#endif