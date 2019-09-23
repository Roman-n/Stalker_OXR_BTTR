// --' OldSerpski Stalker 
// --' Консольный лаунчер на C# с выбором рендера
// --' АМК - наше всё

#define V_ONE
#undef SYSTEM
#undef TIMER_C

using System;
#if TIMER_C
using System.Threading;
#endif

#if SYSTEM
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#endif

namespace timer_c
{
#if TIMER_C
    class Program
    {
        static void Main(string[] args)
        {
            int num = 0;
            // устанавливаем метод обратного вызова
            TimerCallback tm = new TimerCallback(Count);
            // создаем таймер
            Timer timer = new Timer(tm, num, 0, 2000);

            Console.ReadLine();
        }
        public static void Count(object obj)
        {
            int x = (int)obj;
            for (int i = 1; i < 9; i++, x++)
            {
                Console.WriteLine("{0}", x * i);
            }
        }
    }
#endif
}

namespace launcher_stalker
{
    class Launcher_class : EventArgs {public char ch;}

    class KeyAction
    {
        public event EventHandler<Launcher_class> KeyDown;
        public void OnKeyDown(char ch)
        {
            Launcher_class c = new Launcher_class();
            if (KeyDown != null)
            {
                c.ch = ch;
                KeyDown(this, c);
            }
        } 
    }

    class Base
    {
        static void Main()
        {
            KeyAction evnt = new KeyAction();
            evnt.KeyDown += (sender, e) =>
            {
                switch (e.ch)
                {
                    case '0':
                        {
                            System.Diagnostics.Process.Start("OpenXRay - DX9a");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case '1':
                        {
                            System.Diagnostics.Process.Start("OpenXRay - DX9");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case '2':
                        {
                            System.Diagnostics.Process.Start("OpenXRay - DX9c");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case '3':
                        {
                            System.Diagnostics.Process.Start("OpenXRay - DX9_5");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case '4':
                        {
                            System.Diagnostics.Process.Start("OpenXRay - DX10");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case '5':
                        {
                            System.Diagnostics.Process.Start("OpenXRay - DX11");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case '6':
                        {
//                            System.Diagnostics.Process.Start("OpenXRay - OpenGL");
                            CC(ConsoleColor.Red);
                            Console.WriteLine("\nРендер OpenGL отключен, команда не доступна");
//                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }

                    case 's':
                        {
                            System.Diagnostics.Process.Start("OpenXRay");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                        }
                    
                     case 'c':
                       {
                            System.Diagnostics.Process.Start("OpenXRay - console");
                            CC(ConsoleColor.Cyan);
                            Console.WriteLine("\nСтарт... обновление ввода команды");
                            break;
                       }

                    case '7':
                        {
                            Environment.Exit(0);
                            break;
                        }

                    default:
                        {
                            CC(ConsoleColor.Red);
                            Console.WriteLine("\nОшибка ввода команды");
                            break;
                        }
                }
            };

           ConsoleTitle(); char ch; do
            {
                CC(ConsoleColor.Green);

                Console.Write("Введите ключ команды: ");

                ConsoleKeyInfo key;

                {                               //--' Чтобы окно не было мелким
                    Console.BufferHeight = 100;
                    Console.WindowHeight = 40;
                    Console.BufferWidth = 100; // Высота по x от прямой 
                    Console.WindowWidth = 100; // Ширина по y от угла
                }

                key = Console.ReadKey();

                ch = key.KeyChar;

                evnt.OnKeyDown(key.KeyChar);

            }
            while (ch != '7');
        }
        // Описание в консоли
        static void ConsoleTitle()
        {
            CC(ConsoleColor.Yellow);

            Console.WriteLine(

            "****************************************************************\n"
#if V_ONE
            + "\nS.T.A.L.K.E.R.: Call of Chernobyl [OpenXRay]: Back to the roots\n"
#else
			+ "\nS.T.A.L.K.E.R.: Call of Chernobyl [OpenXRay]: 2018\n"
#endif
            + "\nОриентир на новое, с памятью о старом. (с) Collector\n"

            + "\n****************************************************************\n");

            CC(ConsoleColor.Yellow);

            Console.WriteLine("Информация о запуске игры с определенным ключом: \n");

            Command("0", "DirectX 9a");

            Command("1", "DirectX 9");

            Command("2", "DirectX 9с");

            Command("3", "DirectX 9.5");

            Command("4", "DirectX 10");

            Command("5", "DirectX 11");

            Command("6", "Render OpenGL - отключен(!)");

            Command1("s", "Запуск без ключей рендеров, разрешены все команды");
 
            Command1("c", "Запуск игры с разрешенной консолью, без ключа рендера");

            Command1("7", "Выход из лаунчера");

            Console.WriteLine();

        }
                                               // --' Важное, запуск рендеров
            static void CC(ConsoleColor color)
                {
                    Console.ForegroundColor = color;
                }
            static void Command(string s1, string s2)
                {
                    CC(ConsoleColor.Green); Console.Write(s1); CC(ConsoleColor.Red); Console.Write(" - " + s2 + "\n");
                }
                                              // --' Другие ключи запуска
            static void CC1(ConsoleColor color)
                {
                    Console.ForegroundColor = color;
                }
            static void Command1(string s1, string s2)
                {
                    CC1(ConsoleColor.Yellow); Console.Write(s1); CC1(ConsoleColor.Cyan); Console.Write(" - " + s2 + "\n");
                }
    }
}

