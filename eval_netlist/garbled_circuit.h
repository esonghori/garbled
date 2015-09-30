/*
 This file is part of JustGarble.

 JustGarble is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 JustGarble is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with JustGarble.  If not, see <http://www.gnu.org/licenses/>.

 */
/*
 This file is part of TinyGarble. It is modified version of JustGarble
 under GNU license.

 TinyGarble is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TinyGarble is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TinyGarble.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file justGarble.h
 * @author JustGarble & Ebi
 * @date 27 Mar 2015
 * @brief File containing the definition of main structures and functions
 *
 */

#ifndef EVAL_NETLIST_GARBLED_CIRCUIT_H_
#define EVAL_NETLIST_GARBLED_CIRCUIT_H_

#include "block.h"

/**
 * @brief Used to store two tokens (or labels).
 *
 * In garbling, both labels are used, while in evaluation,
 * only one label is required which is label0.
 */
typedef struct Wire {
  block label0; /**< label for 0, and unknown. */
  block label1; /**< label for 1. */
} Wire;

/**
 * @brief Used to store inputs, output, and type of gate in the circuit.
 *
 */
typedef struct GarbledGate {
  long input0; /**< wire index for 1st input. */
  long input1; /**< wire index for 2st input. */
  long output; /**< wire index for output. */
  int type; /**< wire Type, defined in common.h */
} GarbledGate;

/**
 * @brief Stores Garbled Circuit.
 *
 * GarbledCircuit structure stores all the information required
 * for both garbling and evaluation. It is created based on SCD file.
 */
typedef struct GarbledCircuit {
  int n; /*!< # of inputs */
  int g; /*!< # of g inputs */
  int p; /*!< # of DFF */
  int m; /*!< # of outputs */
  int q; /*!< # of gates */
  int c; /*!< # of sequential cycle */
  int r; /*!< # of wires = n+p+q */

  GarbledGate* garbledGates; /*!< topologically sorted gates */
  Wire* wires; /*!< wire labels */
  int *outputs; /*!< index of output wires */
  int *D; /*!< p-length array of wire index corresponding
   to D signal (Data) of DFF. */
  int *I; /*!< p-length array of wire index corresponding
   to I signal (Initial) of DFF. */
  block globalKey; /*!< global key c for AES_c(.) in DKC scheme */
} GarbledCircuit;

/**
 * @brief Create 2*n input labels and initial DFF labels.
 *
 * The function generates n random pair token. Token 0 is
 * generated randomly, token 1 is generated by token0 ^ R.
 * inputLabels[2*i + 0] represent token0 for input i and
 * inputLabels[2*i + 1] represent token1 for input i.
 *
 * @param param1 pointer to allocated 2*n inputLables.
 * @param param2 Garbling secret random block R.
 * @param param3 Number of pain tokens.
 *
 * @see JustGarble paper.
 *
 */
void createInputLabels(block* inputLabels, block R, int n);

/**
 * @brief Garble the circuit described in garbledCircuit.
 *
 * For efficiency reasons, we use the garbledCircuit data-structure for representing the input
 * circuit and the garbled output. The garbling process is non-destructive and
 * only affects the garbledTable member of the GarbledCircuit data-structure.
 * In other words, the same GarbledCircuit object can be reused multiple times,
 * to create multiple garbled circuit copies,
 * by just switching the garbledTable field to a new one. Also, the garbledTable
 * field is the only one that should be sent over the network in the case of an
 * MPC-type application, as the topology is expected to be available to the
 * receiver, and the gate-types are to be hidden away.
 * The inputLabels field is expected to contain 2n fresh input labels, obtained
 * by calling createInputLabels. The outputMap is expected to be a 2m-block sized
 * empty array.
 *
 * Half-Gate is replaced with the old code in JustGarble.
 *
 * @param param1 pointer to read garbledCircuit.
 * @param param2 pointer to generated pair inputLabels.
 * @param param3 pointer to generated pair initialDFFLabels.
 * @param param4 pointer to allocated pair output. The output labels will be filled.
 * @param param5 garbling secret random block R.
 * @param param6 opened socket to evaluator.
 * @return Number of clock cycle measure with RDTSC for garbling
 *
 * @see JustGarble paper.
 * @see Half-Gate paper.
 */
long garble(GarbledCircuit *garbledCircuit, block* inputLabels,
            block* initialDFFLabels, block* outputLabels, block R, int connfd);

/**
 * @brief Evaluate a garbled circuit
 *
 * Evaluate a garbled circuit, using n input labels in the Extracted Labels
 * to return m output labels. The garbled circuit might be generated either in
 * one piece, as the result of running garbleCircuit, or may be pieced together,
 * by building the circuit (startBuilding ... finishBuilding), and adding
 * garbledTable from another source, say, a network transmission.
 *
 * Half-Gate is replaced with the old code in JustGarble.
 *
 * @param param1 pointer to read garbledCircuit.
 * @param param2 pointer to generated inputLabels.
 * @param param3 pointer to generated initialDFFLabels.
 * @param param4 pointer to allocated output. The output labels will be filled..
 * @param param5 opened socket to garbler.
 * @return Number of clock cycle measure with RDTSC for garbling
 *
 * @see JustGarble paper.
 * @see TinyGarble paper.
 * @see Half-Gate paper.
 */

long evaluate(GarbledCircuit *garbledCircuit, block* inputLables,
              block* initialDFFLabels, block *outputs, int connfd);

/**
 * @brief Deallocates garbledCircuit
 *
 *
 * @param param1 pointer to garbledCircuit. The garbledCircuit will be deallocated.
 */
void removeGarbledCircuit(GarbledCircuit *garbledCircuit);

#endif /* EVAL_NETLIST_GARBLED_CIRCUIT_H_ */
