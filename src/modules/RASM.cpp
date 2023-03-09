//
// Created by aanas on 12/12/22.
//
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::initializeInterpreterThread() {
    manualProgram_.clear();
    progThread_ = QtConcurrent::run(this, &MainWindow::rasmInterpreter);
}

/*void MainWindow::checkIfFiledialog() {
    if (fileopen_) {
        filename_ = QFileDialog::getOpenFileName(this,
                                                 "Pick the initializeInterpreterThread file: ", "./",
                                                 "Compiled robot Assembly Files (*.bin)");

        if (filename_.size() == 0) {
            //jogging_ = false;
            //fileopen_ = false;
        }
    }
}*/

void MainWindow::loadProgram()
{
    setButtonColor(3);
    setCameraBarVisibility(false);
    setLearnBarVisibility(false);
}

#include <exception>
#include <chrono>
#include <thread>

inline void MainWindow::fetchDecodeExecute_(Instruction instruction, int& program_counter)
{
    std::string deb;

    switch (instruction.opcode) {
        case ANG:
            Logger::info("Requested joint " + std::to_string(instruction.params.at(0)) + " at " +
                         std::to_string(instruction.params.at(1)));
            switch (static_cast<int>(instruction.params.at(0))) {
                case 1:
                    ui_->base_r->setValue(instruction.params.at(1));
                    break;
                case 2:
                    ui_->a2_r->setValue(instruction.params.at(1));
                    break;
                case 3:
                    ui_->a3_r->setValue(instruction.params.at(1));
                    break;
                case 4:
                    ui_->a4_r->setValue(instruction.params.at(1));
                    std::cout << ui_->a4_r->value();
                    break;
                case 5:
                    ui_->a5_r->setValue(instruction.params.at(1));
                    break;
                default:
                case 6:
                    ui_->increment_6->setValue(instruction.params.at(0));
                    break;
            }
            usleep(timeMod_ * 1000);
            break;

        case ANGS: {
            if (instruction.params.at(0) == 8192) {
                deb.clear();

                std::vector<float> t = instruction.params;
                for(int i = 0 ; i < t.size(); i++)
                    t.at(i) = (int)(floor(t.at(i)));
                t.erase(t.begin());
                for (auto &i: t) {
                    deb.append(std::to_string((i)));
                    deb.append(" ");
                }
                Logger::info("Requested " + deb);
                dev_.servoWrite6(t, 1000);

            } else {
                deb.clear();
                Logger::info("Requested goto target " + std::to_string((int) instruction.params.at(0)));
                auto t = target_variables.at((int) instruction.params.at(0)).angles.data();
                //dev_.servoWrite6(t, timeMod_);
            }
            // a poor man's delay that can be interrupted

            int time_slept = 0;
            int division = 10;
            std::cout << timeMod_ << '\n';
            while (time_slept < timeMod_) {
                if (interruptFlag_){
                    interruptFlag_ = false;
                    break;
                }
                time_slept += division;
                std::this_thread::sleep_for(std::chrono::milliseconds(division));
            }

            break;
        }
        case DEL:
            usleep(instruction.params.at(0) * 1000);
            break;

        case OFS: // TODO
            break;

        case SPD:
            timeMod_ = instruction.params.at(0);
            break;

        case GHME:
            //this -> goHome();
            usleep(timeMod_ * 1000);
            break;

        case SHME: // TODO
            break;

        case DEC:
            instruction.params.at(1) = -1 * instruction.params.at(1);

        case INC:
            switch (static_cast<int>(instruction.params.at(0))) {
                case 1: {
                    int orig = ui_->base_r->value();
                    ui_->base_r->setValue(orig + instruction.params.at(1));
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
            }
            usleep(timeMod_ * 1000);
            break;

        case MOVJ:
            break;
        case MOVL:
            break;
        case END:
            break;
        case GOTO:
            program_counter = instruction.params.at(0) - 1;
        case IF: {
            switch (static_cast<int>(instruction.params.at(1))) {
                case LE:
                    if (numeric_variables[instruction.params.at(0)].value <=
                        numeric_variables[instruction.params.at(2)].value)
                        program_counter = instruction.params.at(3) - 1;
                    break;
                case L:
                    if (numeric_variables[instruction.params.at(0)].value <
                        numeric_variables[instruction.params.at(2)].value)
                        program_counter = instruction.params.at(3) - 1;
                    break;
                case GE:
                    if (numeric_variables[instruction.params.at(0)].value >=
                        numeric_variables[instruction.params.at(2)].value)
                        program_counter = instruction.params.at(3) - 1;
                    break;
                case G:
                    if (numeric_variables[instruction.params.at(0)].value >
                        numeric_variables[instruction.params.at(2)].value)
                        program_counter = instruction.params.at(3) - 1;
                    break;
                case EQ:
                    if (numeric_variables[instruction.params.at(0)].value ==
                        numeric_variables[instruction.params.at(2)].value)
                        program_counter = instruction.params.at(3) - 1;
                    break;
            }
        }
            break;
        case ABR:
            break;
        case NUMERIC_VAR: {
            if (instruction.params.at(0) + 1 > numeric_variables.size()) {
                std::cout << "Need more space!\n";
                std::cout << instruction.params.at(0) << " " << numeric_variables.size() << '\n';
                numeric_variables.reserve(numeric_variables.size() + 1);
            }
            numeric_variables[instruction.params.at(0)].value = instruction.params.at(1);
            break;
        }

        case TGT: {

            variable::Target tgt;
            for (int i = 1; i < instruction.params.size(); i++)
                tgt.angles.push_back(instruction.params.at(i));


            if( instruction.params.at(0) == -1 || (instruction.params.at(0) >= target_variables.size()))
                target_variables.push_back(tgt);
            else
            {
                target_variables.at(instruction.params.at(0)) = tgt;
            }

            deb.clear();
            std::cout << target_variables.size() << '\n';
            for (auto &i: target_variables.back().angles) {
                deb.append(std::to_string(i));
                deb.append(" ");
            }
            Logger::info("Registered ROB-TARGET " + std::to_string(int(instruction.params.at(0))) + " at " + deb);

            break;
        }

        case ADD: {
            numeric_variables[instruction.params.at(0)].value =
                    numeric_variables[instruction.params.at(1)].value +
                    numeric_variables[instruction.params.at(2)].value;
            break;
        }
        case PRT: {
            std::cout << numeric_variables[instruction.params.at(0)].value << '\n';
            std::cout.flush();
            break;
        }
        case SUB: {
            numeric_variables[instruction.params.at(0)].value =
                    numeric_variables[instruction.params.at(1)].value -
                    numeric_variables[instruction.params.at(2)].value;
            break;
        }
        case DIV: {
            numeric_variables[instruction.params.at(0)].value = floor(
                    numeric_variables[instruction.params.at(1)].value /
                    numeric_variables[instruction.params.at(2)].value);
            break;
        }
        case FDIV: {
            numeric_variables[instruction.params.at(0)].value =
                    numeric_variables[instruction.params.at(1)].value /
                    numeric_variables[instruction.params.at(2)].value;
            break;
        }

        case SQRT: {
            numeric_variables[instruction.params.at(0)].value = sqrt(
                    numeric_variables[instruction.params.at(0)].value);
            break;
        }

        case TRNC: {
            numeric_variables[instruction.params.at(0)].value = floor(
                    numeric_variables[instruction.params.at(0)].value);
            break;
        }
    }
}


// Warning! Under construction

void MainWindow::rasmInterpreter()
{

    Logger::info("Interpreter active");

    if (interpreterActive)
        Logger::error("An attempt was made to start a new interpreter instance while one was already running");

    interpreterActive = true;

    //jogging_ = !jogging_;
    //toggleJog();
    dev_.toggleTorque(true);

    //std::vector<Instruction> programStack_;

    int program_counter = 0;
    int program_size = programStack_.size();

    while (running_) {
        if ( !programInLoop_ ) {
            Logger::info("Reached and locked");
            interpreterLock_.lock();
            Logger::info("Interpreter Unlocked");
        }
        program_counter = 0;
        if( oneShot_.opcode != 254 ) {
            fetchDecodeExecute_(oneShot_, program_counter);
            oneShot_.opcode = 254;
            oneShot_.params.clear();
        }
        else {
            while (program_counter < programStack_.size() && running_) {

                Instruction instruction = programStack_.at(program_counter);
                fetchDecodeExecute_(instruction, program_counter);
                program_counter++;
            }
        }
    }
    Logger::info("Interpreter has exited");
}



/*switch (programStack_.size())
{
    case 0:
    {
        fileopen_ = true;

        while (filename_.length() == 0 && jogging_)
        {}

        fileopen_ = false;

        //std::fstream input_file(filename.toStdString(), std::ios::in | std::ios::binary);
        std::fstream input_file(filename_.toStdString(), std::ios::in);

        std::string progname;
        std::string progsize_s;

        input_file >> progname;
        size_t progsize;
        input_file >> progsize;

        input_file.get(); // remove trailing \n
        uint16_t read_size = 0;
        ////while (read_size < progsize)
        {
            char *chunk = new char[sizeof(Instruction)];
            input_file.read(chunk, sizeof(Instruction));
            Instruction a = *(Instruction *)chunk;
            programStack_.push_back(*(Instruction *)chunk);
            delete[] chunk;
            read_size += sizeof(Instruction);
        }//// < block comment
        while(!input_file.eof())
        {
            Instruction a; a.params.clear();
            int c;
            input_file >> c;
            a.opcode = c;

            for(int i = 0 ; i < 10 ; i ++)
            {
                float b;
                input_file >> b;
                a.params.push_back(b);
            }
            programStack_.push_back(a);
        }
        break;
    }
    default:
    {
        programStack_ = programStack_;
        break;
    }
}
*/