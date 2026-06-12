# Modern WPF MVVM Architecture Guide (AI Prompt / CursorRules Template)

This document is a comprehensive architectural guide for AI coding agents (GitHub Copilot, Cursor, Claude, etc.) to reference when building modern WPF (.NET 8.0+) applications. It is designed to strictly forbid common AI-generated anti-patterns and enforce 100% utilization of the CommunityToolkit.Mvvm source generators.

---

## 1. Project Directory Structure

To maximize separation of concerns and maintainability, adhere strictly to the following directory layout:

```text
📁 MyWpfApp/
├── 📁 Config/           # Application configuration files (JSON, XML, settings, etc.)
├── 📁 Models/           # Pure POCO data models, database/API entities
├── 📁 Services/         # Business logic, API clients, File I/O, DB access layer
│   ├── IApiService.cs
│   └── ApiService.cs
├── 📁 ViewModels/       # UI presentation logic (inheriting from CommunityToolkit.Mvvm)
│   ├── MainViewModel.cs
│   └── SubViewModel.cs
├── 📁 Views/            # XAML UI layouts and minimal behind code
│   ├── MainWindow.xaml
│   └── MainWindow.xaml.cs
├── 📁 Resources/        # Static assets (icons, images, style dictionaries, etc.)
├── App.xaml             # Application entry point and global resource definitions
├── App.xaml.cs          # Dependency injection container setup and startup logic
└── MyWpfApp.csproj      # Modern SDK-style project file
```

---

## 2. Dependency Injection (DI) Configuration (`App.xaml.cs`)

Use `Microsoft.Extensions.DependencyInjection` to configure the global IoC container.

```csharp
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Windows;
using MyWpfApp.Services;
using MyWpfApp.ViewModels;
using MyWpfApp.Views;

namespace MyWpfApp
{
    public partial class App : Application
    {
        // Expose the DI container globally so views can resolve services when necessary
        public new static App Current => (App)Application.Current;
        public IServiceProvider Services { get; }

        public App()
        {
            Services = ConfigureServices();
        }

        private static IServiceProvider ConfigureServices()
        {
            var services = new ServiceCollection();

            // 1. Register Core Services (Typically Singleton)
            services.AddSingleton<ISettingsService, JsonSettingsService>();
            services.AddHttpClient<IApiService, ApiService>(); // Factory pattern

            // 2. Register ViewModels
            services.AddSingleton<MainViewModel>(); // Application-wide Singleton ViewModel
            services.AddTransient<SettingsViewModel>(); // Transient (new instance per request)

            // 3. Register Views
            services.AddTransient<MainWindow>();

            return services.BuildServiceProvider();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            var mainWindow = Services.GetRequiredService<MainWindow>();
            mainWindow.DataContext = Services.GetRequiredService<MainViewModel>();
            mainWindow.Show();
        }
    }
}
```

---

## 3. Core Compliance Rules & Anti-Patterns (Strictly Enforced)

AI agents must strictly adhere to the following constraints and rules when writing C# and XAML code (these rules should be copied directly into agent prompts or `.cursorrules` files):

### 🚨 [Rule 1] No Dependency Injection (DI) Anti-Patterns
* **Strictly Forbidden (Service Locator Pattern):** Do not call `Ioc.Default.GetService<T>()` or `App.Current.Services.GetRequiredService<T>()` inside ViewModels.
* **Solution:** All external services, repositories, and sub-viewmodels **must be injected via Constructor Injection**.
* *Exception:* Code-behind files of Views (`.xaml.cs`) are allowed to resolve their corresponding ViewModels using `App.Current.Services` since they lack parameter constructors for XAML instantiation.

### 🚨 [Rule 2] CommunityToolkit.Mvvm Source Generator Rules
* **Strictly Forbidden:** Do not manually implement the `INotifyPropertyChanged` interface.
* **Mandatory:** All ViewModels must inherit from `ObservableObject` and must be declared with the `partial` keyword.
* **Field Naming:** Private backing fields for properties must be declared in `camelCase` or `_camelCase` and annotated with `[ObservableProperty]`. Do not name backing fields in `PascalCase`, as this causes compilation collisions with the generated public properties.
* **Dependent Properties:** If a property update requires a UI refresh on a derived/dependent property, do not raise property-changed events manually in the setter. Instead, annotate the source backing field with `[NotifyPropertyChangedFor(nameof(DependentPropertyName))]`.

### 🚨 [Rule 3] Avoid `async void` in Commands
* **Strictly Forbidden:** Never declare asynchronous command handlers as `async void`. (Exceptions thrown in `async void` are uncatchable and will crash the application.)
* **Solution:** All asynchronous command methods must return `Task`. Annotating them with `[RelayCommand]` automatically generates a safe `IAsyncRelayCommand`.
* **State Synchronization:** If a command's execution state (`CanExecute`) depends on another property, annotate that property's backing field with `[NotifyCanExecuteChangedFor(nameof(CommandNameCommand))]`.

### 🚨 [Rule 4] Prevent Memory Leaks in IMessenger
* **Strictly Forbidden:** Do not register a `WeakReferenceMessenger` callback with an instance method lambda that captures `this`, as it prevents garbage collection and causes memory leaks.
* **Solution:** Always use a static lambda callback: e.g., `(r, m) => r.OnMessage(m)`. Alternatively, inherit from `ObservableRecipient`, implement `IRecipient<TMessage>`, and invoke `Messenger.RegisterAll(this)`.

---

## 4. Perfect ViewModel Template Example

This template demonstrates a correct implementation adhering to all the rules above. AI agents must replicate this design pattern when creating new ViewModels.

```csharp
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Threading.Tasks;
using MyWpfApp.Services;

namespace MyWpfApp.ViewModels
{
    // [Rule 2] Must be a partial class and inherit from ObservableObject
    public partial class MainViewModel : ObservableObject
    {
        private readonly IApiService _apiService;

        // [Rule 1] Inject dependencies through the constructor (No Service Locator pattern)
        public MainViewModel(IApiService apiService)
        {
            _apiService = apiService;
            Title = "Modern WPF MVVM Template";
        }

        // [Rule 2] Private fields are _camelCase, annotated with [ObservableProperty]
        // The public 'Title' property is generated automatically during compilation
        [ObservableProperty]
        private string _title = string.Empty;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(SubmitCommand))] // [Rule 3] Re-evaluate CanExecute when IsProcessing changes
        private bool _isProcessing;

        private bool CanSubmit() => !IsProcessing;

        // [Rule 3] async void is strictly forbidden. Return Task instead.
        // The public 'SubmitCommand' (IAsyncRelayCommand) is generated automatically.
        [RelayCommand(CanExecute = nameof(CanSubmit))]
        private async Task SubmitAsync()
        {
            IsProcessing = true;
            try
            {
                await _apiService.SendDataAsync(Title);
            }
            finally
            {
                IsProcessing = false;
            }
        }
    }
}
```
