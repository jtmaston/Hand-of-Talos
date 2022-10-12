#include "mainwindow.hpp"
#include "ui_mainwindow.h"


using namespace std::chrono;


void MainWindow::rasmInterpreter(const std::vector<float> &homePosition, std::vector<Instruction> *programStack,
                                 const bool *running) // TODO: memory optimizations
{

    std::vector<Variable::Numeric> numeric_variables;
    std::vector<Variable::Target> target_variables;

    Logger::info("Interpreter active_!");


    while (*running) {
        antiFreewheel_.lock();
        auto program_size = (int) programStack->size();
        int program_counter = 0;
        while ((program_counter < program_size) && *running) {
            Instruction instruction = programStack->at(program_counter);
            switch (programStack->at(program_counter).opcode) {
                case ANG:
                    switch (static_cast<int>(instruction.params.at(0))) {
                        case 1:
                            ui_->base_r->setValue((int) instruction.params.at(1));
                            break;
                        case 2:
                            ui_->a2_r->setValue((int) instruction.params.at(1));
                            break;
                        case 3:
                            ui_->a3_r->setValue((int) instruction.params.at(1));
                            break;
                        case 4:
                            ui_->a4_r->setValue((int) instruction.params.at(1));
                            std::cout << ui_->a4_r->value();
                            break;
                        case 5:
                            ui_->a5_r->setValue((int) instruction.params.at(1));
                            break;
                        case 6:
                            ui_->grip_r->setValue((int) instruction.params.at(1));
                            break;
                    }
                    usleep(dev_.timeFactor_ * 1000);
                    break;

                case ANGS:
                    switch ((int) instruction.params.at(0)) {
                        case 8192: {
                            // auto test = dev.servoReadall();

                            // wd.source.clear();
                            // wd.destination.clear();

                            /*for( uint_fast16_t i = 0 ; i < 6; i++ )
                            {
                                wd.source.push_back(test[i));
                                wd.destination.push_back(instruction.params.at(i + 1));
                            }*/

                            // delete test;

                            // dev.servoWrite6(instruction.params + 1, time_mod);       FIXME: uncomment
                            dev_.setDestination(instruction.params);

                            break;
                        }
                        default: // in theory, this *should* maintain backwards compatibility with older programs. Maybe.
                        {
                            // dev.servoWrite6(target_variables.at(instruction.params.at(0)).angles, time_mod); FIXME: uncomment
                            break;
                        }
                    }
                    if (!waitForMoveComplete_)
                        usleep(dev_.timeFactor_ * 1000);
                    break;

                case DEL:
                    usleep(instruction.params.at(0) * 1000);
                    break;

                case OFS: // TODO
                    break;

                case SPD:
                    dev_.timeFactor_ = instruction.params.at(0);
                    break;

                case GHME:
                    // this -> go_home();
                    usleep(dev_.timeFactor_ * 1000);
                    break;

                case SHME: // TODO
                    break;

                case DEC:
                    instruction.params.at(1) = -1 * instruction.params.at(1);

                case INC:
                    /*switch (static_cast<int>(instruction.params.at(0))) {
                        case 1: {
                            int orig = ui_->base_r->value();
                            ui_->base_r->setValue((int)orig + instruction.params.at(1));
                            break;
                        }
                        case 2: {
                            int orig = ui_->a2_r->value();
                            ui_->a2_r->setValue(orig + instruction.params.at(1));
                            break;
                        }
                        case 3: {
                            int orig = ui_->a3_r->value();
                            ui_->a3_r->setValue(orig + instruction.params.at(1));
                            break;
                        }
                        case 4: {
                            int orig = ui_->a4_r->value();
                            ui_->a4_r->setValue(orig + instruction.params.at(1));
                            break;
                        }
                        case 5: {
                            int orig = ui_->a5_r->value();
                            ui_->a5_r->setValue(orig + instruction.params.at(1));
                            break;
                        }
                        case 6: {
                            int orig = ui_->grip_r->value();
                            ui_->grip_r->setValue(orig + instruction.params.at(1));
                            break;
                        }
                    }*/
                    // TODO: reimplement
                    usleep(dev_.timeFactor_ * 1000);
                    break;

                case MOVJ:
                    break;
                case MOVL:
                    break;
                case END:
                    break;
                case GOTO:
                    program_counter = (int) instruction.params.at(0) - 1;
                case IF: {
                    switch (static_cast<int>(instruction.params.at(1))) {
                        case LE:
                            if (numeric_variables.at((int) instruction.params.at(0)).value <=
                                numeric_variables.at((int) instruction.params.at(2)).value)
                                program_counter = (int) instruction.params.at(3) - 1;
                            break;
                        case L:
                            if (numeric_variables.at((int) instruction.params.at(0)).value <
                                numeric_variables.at((int) instruction.params.at(2)).value)
                                program_counter = (int) instruction.params.at(3) - 1;
                            break;
                        case GE:
                            if (numeric_variables.at((int) instruction.params.at(0)).value >=
                                numeric_variables.at((int) instruction.params.at(2)).value)
                                program_counter = (int) instruction.params.at(3) - 1;
                            break;
                        case G:
                            if (numeric_variables.at((int) instruction.params.at(0)).value >
                                numeric_variables.at((int) instruction.params.at(2)).value)
                                program_counter = (int) instruction.params.at(3) - 1;
                            break;
                        case EQ:
                            if (numeric_variables.at((int) instruction.params.at(0)).value ==
                                numeric_variables.at((int) instruction.params.at(2)).value)
                                program_counter = (int) instruction.params.at(3) - 1;
                            break;
                    }
                }
                    break;
                case ABR:
                    break;
                case NUMERIC_VAR: {
                    if ((instruction.params.at(0) + 1) > (float) numeric_variables.size()) {
                        numeric_variables.reserve(numeric_variables.size() + 1);
                    }
                    numeric_variables.at((int) instruction.params.at(0)).value = instruction.params.at(1);
                    break;
                }

                case TGT: {
                    if (instruction.params.at(0) + 1 > (float) target_variables.size())
                        target_variables.reserve(target_variables.size() + 1);
                    Variable::Target tgt;
                    tgt.name = (int) instruction.params.at(0);
                    memcpy(tgt.angles.data(), instruction.params.data() + 1, 5 * sizeof(float));
                    target_variables.push_back(tgt);

                    break;
                }

                case ADD: {
                    numeric_variables.at((int) instruction.params.at(0)).value =
                            numeric_variables.at((int) instruction.params.at(1)).value +
                            numeric_variables.at((int) instruction.params.at(2)).value;
                    break;
                }
                case PRT: {
                    std::cout << numeric_variables.at((int) instruction.params.at(0)).value << '\n';
                    std::cout.flush();
                    break;
                }
                case SUB: {
                    numeric_variables.at((int) instruction.params.at(0)).value =
                            numeric_variables.at((int) instruction.params.at(1)).value -
                            numeric_variables.at((int) instruction.params.at(2)).value;
                    break;
                }
                case DIV: {
                    numeric_variables.at((int) instruction.params.at(0)).value = floor(
                            numeric_variables.at((int) instruction.params.at(1)).value /
                            numeric_variables.at((int) instruction.params.at(2)).value);
                    break;
                }
                case FDIV: {
                    numeric_variables.at((int) instruction.params.at(0)).value =
                            numeric_variables.at((int) instruction.params.at(1)).value /
                            numeric_variables.at((int) instruction.params.at(2)).value;
                    break;
                }

                case SQRT: {
                    numeric_variables.at((int) instruction.params.at(0)).value = sqrt(
                            numeric_variables.at((int) instruction.params.at(0)).value);
                    break;
                }

                case TRNC: {
                    numeric_variables.at((int) instruction.params.at(0)).value = floor(
                            numeric_variables.at((int) instruction.params.at(0)).value);
                    break;
                }
            }
            program_counter++;
        }
        programStack->clear();
    }

    Logger::info("Exited interpreter!");
}