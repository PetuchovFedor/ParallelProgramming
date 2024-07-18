using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        const string apiUrl = "https://dog.ceo/api/breeds/image/random";

        Console.WriteLine("Синхронная загрузка:");
        var syncWatch = System.Diagnostics.Stopwatch.StartNew();
        DownloadDogImagesSync(apiUrl, 10);
        syncWatch.Stop();
        Console.WriteLine($"Время выполнения синхронной загрузки: {syncWatch.ElapsedMilliseconds} мс");

        Console.WriteLine("\nАсинхронная загрузка:");
        var asyncWatch = System.Diagnostics.Stopwatch.StartNew();
        await DownloadDogImagesAsync(apiUrl, 10);
        asyncWatch.Stop();
        Console.WriteLine($"Время выполнения асинхронной загрузки: {asyncWatch.ElapsedMilliseconds} мс");
    }

    static async Task DownloadDogImagesAsync(string apiUrl, int count)
    {
        var httpClient = new HttpClient();
        var tasks = new List<Task>();

        for (int i = 0; i < count; i++)
        {
            var i1 = i;
            //var nameImg = $"img{i1 + 1}.jpg";
            tasks.Add(Task.Run(async () =>
            {
                HttpResponseMessage response = await httpClient.GetAsync(apiUrl);
                if (response.IsSuccessStatusCode)
                {
                    string responseBody = await response.Content.ReadAsStringAsync();                   
                    Console.WriteLine($"Image {i1 + 1}: {responseBody}");
                }
                else
                {
                    Console.WriteLine($"Не удалось загрузить изображение {i + 1}. Статус ответа: {response.StatusCode}");
                }
            }));
        }

        await Task.WhenAll(tasks);
    }

    static void DownloadDogImagesSync(string apiUrl, int count)
    {
        var httpClient = new HttpClient();

        for (int i = 0; i < count; i++)
        {
            //var nameImg = $"Simg{i + 1}.jpg";
            HttpResponseMessage response = httpClient.GetAsync(apiUrl).Result;
            if (response.IsSuccessStatusCode)
            {
                string responseBody = response.Content.ReadAsStringAsync().Result;
                Console.WriteLine($"Image {i + 1}: {responseBody}");                                                           
            }
            else
            {
                Console.WriteLine($"Не удалось загрузить изображение {i + 1}. Статус ответа: {response.StatusCode}");
            }
        }
    }
}