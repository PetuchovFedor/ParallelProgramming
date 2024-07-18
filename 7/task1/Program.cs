using System;
using System.IO;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Введите путь к файлу:");
        string filePath = Console.ReadLine();

        if (!File.Exists(filePath))
        {
            Console.WriteLine("Файл не найден.");
            return;
        }

        Console.WriteLine("Введите список символов для удаления:");
        string charsToDelete = Console.ReadLine();
        string[] arrayOfChar = charsToDelete.Split(' ');
        try
        {
            string fileContent = await File.ReadAllTextAsync(filePath);

            foreach (string str in arrayOfChar)
            {
                fileContent = fileContent.Replace(str, "");
            }

            await File.WriteAllTextAsync(filePath, fileContent);
            Console.WriteLine("Файл успешно обновлен.");
        }
        catch (Exception ex)
        {
            Console.WriteLine("Ошибка: ex.Message");
        }
        //D:\Fedya\ParallelProgramming\7\task1\1.txt
    }
}