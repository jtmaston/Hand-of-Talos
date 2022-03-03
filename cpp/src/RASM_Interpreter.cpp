#include "../lib/mainwindow.h"
#include "../lib/ui_mainwindow.h"

void MainWindow::RASM_Interpreter()
{
    fileopen = true;

    while ( filename.length() == 0 && following_program){}

    fileopen = false;
    
    std::vector<Instruction> program;
    std::vector<variable::Numeric> numeric_variables;
    std::fstream input_file ( filename.toStdString() , std::ios::in  | std::ios::binary);
    //std::fstream input_file("/home/parallels/RASM/RASM Examples/if.bin", std::ios::in | std::ios::binary);

    std::string progname;
    std::string progsize_s;
    std::string numsize_s;
    std::string strsize_s;

    auto start = std::chrono::high_resolution_clock::now();
    input_file >> progname;
    input_file >> progsize_s;   size_t progsize = std::stoi(progsize_s);
    input_file >> numsize_s;    size_t numsize = std::stoi(numsize_s);
    input_file >> strsize_s;    size_t strsize = std::stoi(strsize_s);
    
    input_file.get();               // remove trailing \n

    uint16_t read_size = 0;
    while (read_size < progsize)
    {
        char *chunk = new char[sizeof(Instruction)];
        input_file.read(chunk, sizeof(Instruction));
        program.push_back(std::move(*(Instruction *)chunk));
        delete[] chunk;
        read_size += sizeof(Instruction);
    }

    read_size = 0;
    while (read_size < numsize)
    {
        char *chunk = new char[sizeof(variable::Numeric)];
        input_file.read(chunk, sizeof(variable::Numeric));
        numeric_variables.push_back(std::move(*(variable::Numeric *)chunk));
        delete[] chunk;
        read_size += sizeof(variable::Numeric);
    }



    int program_counter = 0;
    int program_size = program.size();

    while (( program_counter  < program_size ) && following_program)
    {
        Instruction instruction = program[program_counter];

        switch (program[program_counter].opcode)
        {
        case ANG:
            switch (instruction.params[0])
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
            usleep(time_mod * 1000);
            std::cout << "angle\n";
            break;

        case ANGS:
            ui->base_r->setValue(instruction.params[0] - 90);
            ui->a2_r->setValue(instruction.params[1] - 90);
            ui->a3_r->setValue(instruction.params[2] - 90);
            ui->a4_r->setValue(instruction.params[3] - 180);
            ui->a5_r->setValue(instruction.params[4] - 90);
            ui->grip_r->setValue(instruction.params[5] - 90);
            usleep(time_mod * 1000);
            break;

        case DEL:
            usleep(instruction.params[0] * 1000);
            break;

        case OFS: // TODO
            break;

        case SPD:
            time_mod = instruction.params[0];
            break;

        case GHME:
            this -> go_home();
            usleep(time_mod * 1000);
            break;

        case SHME: // TODO
            break;

        case DEC:
            instruction.params[1] = -1 * instruction.params[1];

        case INC:
            switch (instruction.params[0])
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
            usleep(time_mod * 1000);
            break;
            
        case RPP:
            break;
        case IPP:
            break;
        case END:
            break;
        case GOTO:
            program_counter = instruction.params[0] - 1;
        case IF:
            {
                switch(instruction.params[1])
                {
                    case LE:
                        if ( numeric_variables[instruction.params[0]].value <= numeric_variables[instruction.params[2]].value )
                            program_counter = instruction.params[ 3 ] - 3;
                        break;
                    case L:
                        if ( numeric_variables[instruction.params[0]].value < numeric_variables[instruction.params[2]].value  )
                            program_counter = instruction.params[ 3 ] - 3;
                        break;
                    case GE:
                        if ( numeric_variables[instruction.params[0]].value >= numeric_variables[instruction.params[2]].value  )
                            program_counter = instruction.params[ 3 ] - 3;
                        break;
                    case G:
                        if ( numeric_variables[instruction.params[0]].value > numeric_variables[instruction.params[2]].value  )
                            program_counter = instruction.params[ 3 ] - 3;
                        break;
                    case EQ:
                        if ( numeric_variables[instruction.params[0]].value == numeric_variables[instruction.params[2]].value )
                            program_counter = instruction.params[ 3 ] - 3;
                        break;
                }
            }
            break;
        case ABR:
            break;
        case NUMERIC:
        {
            numeric_variables[instruction.params[0]].value = instruction.params[1];
            break;
        }
        case ADD:
        {
            numeric_variables[instruction.params[0]].value =
            numeric_variables[instruction.params[1]].value +
            numeric_variables[instruction.params[2]].value;
            break;
        }
        case PRT:
        {
            std::cout << numeric_variables[instruction.params[1]].value << '\n';
            std::cout.flush();
            break;
        }
        case SUB:
        {
            numeric_variables[instruction.params[0]].value =
            numeric_variables[instruction.params[1]].value -
            numeric_variables[instruction.params[2]].value;
            break;
        }
        case DIV:
        {
            numeric_variables[instruction.params[0]].value = floor(
            numeric_variables[instruction.params[1]].value /
            numeric_variables[instruction.params[2]].value
            );
            break;
        }
        case FDIV:
        {
            numeric_variables[instruction.params[0]].value =
            numeric_variables[instruction.params[1]].value /
            numeric_variables[instruction.params[2]].value;
            break;
        }

        case SQRT:
        {
            numeric_variables[instruction.params[0]].value = sqrt(
            numeric_variables[instruction.params[0]].value);
            break;
        }

        case TRNC:
        {
            numeric_variables[instruction.params[0]].value = floor(numeric_variables[instruction.params[0]].value);
            break;
        }

        }
        program_counter++;
    }

    std::cout << '\n';
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
}