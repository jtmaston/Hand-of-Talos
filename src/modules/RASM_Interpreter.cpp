#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <chrono>
using namespace std::chrono;
#include "Watchdog.hpp"

void MainWindow::rasmInterpreter(const std::vector<float> home_position, std::vector<Instruction> *program_stack,
                                 bool *running) // TODO: memory optimizations
{
    
    std::vector<variable::Numeric> numeric_variables;
    std::vector<variable::Target> target_variables;

    Logger::Info("Interpreter active!");



    while (*running)
    {
        antiFreewheel_.lock();
        int program_size = program_stack->size();
        int program_counter = 0;
        while ((program_counter < program_size) && *running)
        {
            Instruction instruction = program_stack->at(program_counter);
            switch (program_stack->at(program_counter).opcode)
            {
            case ANG:
                switch (static_cast<int>(instruction.params[0]))
                {
                case 1:
                    ui->base_r->setValue(instruction.params[1]);
                    break;
                case 2:
                    ui->a2_r->setValue(instruction.params[1]);
                    break;
                case 3:
                    ui->a3_r->setValue(instruction.params[1]);
                    break;
                case 4:
                    ui->a4_r->setValue(instruction.params[1]);
                    std::cout << ui->a4_r->value();
                    break;
                case 5:
                    ui->a5_r->setValue(instruction.params[1]);
                    break;
                case 6:
                    ui->grip_r->setValue(instruction.params[1]);
                    break;
                }
                usleep(dev_.timeFactor * 1000);
                break;

            case ANGS:
                switch ((int)instruction.params[0])
                {
                case 8192:
                {
                    // auto test = dev.servo_readall();

                    // wd.source.clear();
                    // wd.destination.clear();

                    /*for( uint_fast16_t i = 0 ; i < 6; i++ )
                    {
                        wd.source.push_back(test[i]);
                        wd.destination.push_back(instruction.params[i + 1]);
                    }*/

                    // delete test;

                    // dev.servo_write6(instruction.params + 1, time_mod);       FIXME: uncomment
                    dev_.setDestination(instruction.params);

                    break;
                }
                default: // in theory, this *should* maintain backwards compatibility with older programs. Maybe.
                {
                    // dev.servo_write6(target_variables.at(instruction.params[0]).angles, time_mod); FIXME: uncomment
                    break;
                }
                }
                if (!waitForMoveComplete_)
                    usleep(dev_.timeFactor * 1000);
                break;

            case DEL:
                usleep(instruction.params[0] * 1000);
                break;

            case OFS: // TODO
                break;

            case SPD:
                dev_.timeFactor = instruction.params[0];
                break;

            case GHME:
                // this -> go_home();
                usleep(dev_.timeFactor * 1000);
                break;

            case SHME: // TODO
                break;

            case DEC:
                instruction.params[1] = -1 * instruction.params[1];

            case INC:
                switch (static_cast<int>(instruction.params[0]))
                {
                case 1:
                {
                    int orig = ui->base_r->value();
                    ui->base_r->setValue(orig + instruction.params[1]);
                    break;
                }
                case 2:
                {
                    int orig = ui->a2_r->value();
                    ui->a2_r->setValue(orig + instruction.params[1]);
                    break;
                }
                case 3:
                {
                    int orig = ui->a3_r->value();
                    ui->a3_r->setValue(orig + instruction.params[1]);
                    break;
                }
                case 4:
                {
                    int orig = ui->a4_r->value();
                    ui->a4_r->setValue(orig + instruction.params[1]);
                    break;
                }
                case 5:
                {
                    int orig = ui->a5_r->value();
                    ui->a5_r->setValue(orig + instruction.params[1]);
                    break;
                }
                case 6:
                {
                    int orig = ui->grip_r->value();
                    ui->grip_r->setValue(orig + instruction.params[1]);
                    break;
                }
                }
                usleep(dev_.timeFactor * 1000);
                break;

            case MOVJ:
                break;
            case MOVL:
                break;
            case END:
                break;
            case GOTO:
                program_counter = instruction.params[0] - 1;
            case IF:
            {
                switch (static_cast<int>(instruction.params[1]))
                {
                case LE:
                    if (numeric_variables.at(instruction.params[0]).value <= numeric_variables.at(instruction.params[2]).value)
                        program_counter = instruction.params[3] - 1;
                    break;
                case L:
                    if (numeric_variables.at(instruction.params[0]).value < numeric_variables.at(instruction.params[2]).value)
                        program_counter = instruction.params[3] - 1;
                    break;
                case GE:
                    if (numeric_variables.at(instruction.params[0]).value >= numeric_variables.at(instruction.params[2]).value)
                        program_counter = instruction.params[3] - 1;
                    break;
                case G:
                    if (numeric_variables.at(instruction.params[0]).value > numeric_variables.at(instruction.params[2]).value)
                        program_counter = instruction.params[3] - 1;
                    break;
                case EQ:
                    if (numeric_variables.at(instruction.params[0]).value == numeric_variables.at(instruction.params[2]).value)
                        program_counter = instruction.params[3] - 1;
                    break;
                }
            }
            break;
            case ABR:
                break;
            case NUMERIC_VAR:
            {
                if (instruction.params[0] + 1 > numeric_variables.size())
                {
                    numeric_variables.reserve(numeric_variables.size() + 1);
                }
                numeric_variables.at(instruction.params[0]).value = instruction.params[1];
                break;
            }

            case TGT:
            {
                if (instruction.params[0] + 1 > target_variables.size())
                    target_variables.reserve(target_variables.size() + 1);
                variable::Target tgt;
                tgt.name = instruction.params[0];
                memcpy(tgt.angles, instruction.params.data() + 1, 5 * sizeof(float));
                target_variables.push_back(tgt);

                break;
            }

            case ADD:
            {
                numeric_variables.at(instruction.params[0]).value =
                    numeric_variables.at(instruction.params[1]).value +
                    numeric_variables.at(instruction.params[2]).value;
                break;
            }
            case PRT:
            {
                std::cout << numeric_variables.at(instruction.params[0]).value << '\n';
                std::cout.flush();
                break;
            }
            case SUB:
            {
                numeric_variables.at(instruction.params[0]).value =
                    numeric_variables.at(instruction.params[1]).value -
                    numeric_variables.at(instruction.params[2]).value;
                break;
            }
            case DIV:
            {
                numeric_variables.at(instruction.params[0]).value = floor(
                    numeric_variables.at(instruction.params[1]).value /
                    numeric_variables.at(instruction.params[2]).value);
                break;
            }
            case FDIV:
            {
                numeric_variables.at(instruction.params[0]).value =
                    numeric_variables.at(instruction.params[1]).value /
                    numeric_variables.at(instruction.params[2]).value;
                break;
            }

            case SQRT:
            {
                numeric_variables.at(instruction.params[0]).value = sqrt(
                    numeric_variables.at(instruction.params[0]).value);
                break;
            }

            case TRNC:
            {
                numeric_variables.at(instruction.params[0]).value = floor(numeric_variables.at(instruction.params[0]).value);
                break;
            }
            }
            program_counter++;
        }
        program_stack->clear();
    }

    Logger::Info("Exited interpreter!");

    return;
}

/*
    Note: wtf is this


    switch (instructions.size())
    {
    case 0:
    {
        fileopen = true;

        while (filename.length() == 0 && following_program)
        {}

        fileopen = false;

        std::fstream input_file(filename.toStdString(), std::ios::in | std::ios::binary);

        std::string progname;
        std::string progsize_s;

        input_file >> progname;
        size_t progsize;
        input_file >> progsize;

        input_file.get(); // remove trailing \n
        uint16_t read_size = 0;
        while (read_size < progsize)
        {
            char *chunk = new char[sizeof(Instruction)];
            input_file.read(chunk, sizeof(Instruction));
            program.push_back(std::move(*(Instruction *)chunk));
            delete[] chunk;
            read_size += sizeof(Instruction);
        }
        break;
    }
    default:
    {
        program = instructions;
        break;
    }
    }
    std::cout << "In interpreter\n";
    int program_counter = 0;
    int program_size = program.size();
    */