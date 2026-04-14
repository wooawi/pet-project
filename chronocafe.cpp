#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
using namespace std;

class Player;
class Customer;
class EquipmentItem;

enum class Slot_Equipment {
    CoffeeMachine,
    Wallpaper,
    Chairs,
    Refrigerator,
    ProfessionalEspressoMachine,
    DesignerSofas,
    NeonLighting,
    CoffeeGrinder
};

enum class QuestGoal {
    ServeCustomers,
    SellDrinks,
    OpenEra
};

class Entity {
protected:
    string name;
    int level = 0;
    int reputation = 0;
    int energy = 100;
public:
    virtual ~Entity() = default;
    virtual void displayInfo() const = 0;

    virtual void updateStatus(int level, int reputation) {
        this->level = level;
        this->reputation = reputation;
    }
    bool isActive() const { return energy > 0; }

    const string& getName() const { return name; }
    int getLevel() const { return level; }
    int getReputation() const { return reputation; }
    int getEnergy() const { return energy; }
    void setName(const string& newName) { name = newName; }
};

class Item {
protected:
    string name;
    string description;
    int value;
    bool isStackable;
public:
    Item(const string& n, const string& d, int v, bool stack = false)
        : name(n), description(d), value(v), isStackable(stack) {
    }
    virtual ~Item() = default;
    virtual void use(Player* user) = 0;

    const string& getName() const { return name; }
    const string& getDescription() const { return description; }
    int getValue() const { return value; }
    bool canStack() const { return isStackable; }
};

class Inventory {
    int capacity = 30;
    vector<Item*> items;
public:
    ~Inventory() {
        for (auto* it : items) delete it;
    }

    bool addItem(Item* item) {
        if ((int)items.size() >= capacity) {
            cout << "Инвентарь переполнен.\n";
            delete item;
            return false;
        }
        items.push_back(item);
        cout << "Добавлен предмет: " << item->getName() << "\n";
        return true;
    }

    void removeItem(int index) {
        if (index < 0 || index >= (int)items.size()) return;
        delete items[index];
        items.erase(items.begin() + index);
    }

    Item* getItem(int index) {
        if (index < 0 || index >= (int)items.size()) return nullptr;
        return items[index];
    }

    void displayContents() {
        cout << "=== Инвентарь ===\n";
        if (items.empty()) {
            cout << "Пусто.\n";
            return;
        }
        for (int i = 0; i < (int)items.size(); ++i) {
            cout << i << ") " << items[i]->getName()
                << " - " << items[i]->getDescription()
                << " (" << items[i]->getValue() << " монет)\n";
        }
    }

    int getSize() const { return (int)items.size(); }
};

class DrinkItem : public Item {
public:
    DrinkItem(const string& n, const string& d, int v)
        : Item(n, d, v, false) {
    }

    void use(Player* user) override {
        cout << "Напиток выпит. Немного энергии и вдохновения!\n";
    }
};

namespace DrinksFactory {
    inline DrinkItem* makeEspressoTime() {
        return new DrinkItem("Эспрессо времени",
            "Классика ChronoCafe. Кофейные зерна, вода.",
            40);
    }
    inline DrinkItem* makeChronoLatte() {
        return new DrinkItem("Хроно-латте",
            "Мягкий вкус и теплое послевкусие. Зерна, молоко, сахар.",
            50);
    }
    inline DrinkItem* makeSamuraiMatcha() {
        return new DrinkItem("Самурайский матча",
            "Матча, вода, мед. Гармония терпкости и сладости.",
            55);
    }
    inline DrinkItem* makeVictorianMocha() {
        return new DrinkItem("Викторианский мокко",
            "Зерна, шоколад, молоко. Для эстетов.",
            60);
    }
    inline DrinkItem* makeGalacticCappuccino() {
        return new DrinkItem("Галактический капучино",
            "Зерна, пена, звездный сахар. Немного звёзд в чашке.",
            70);
    }
    inline DrinkItem* makeQuantumCoffee() {
        return new DrinkItem("Квантовый кофе",
            "Квантовые зерна, кристальная вода. Даёт больше опыта.",
            120);
    }
    inline DrinkItem* makeAlienDrink() {
        return new DrinkItem("Инопланетный напиток",
            "Инопланетные зерна, космическое молоко. Шанс редких ресурсов.",
            140);
    }
    inline DrinkItem* makeChronoSupreme() {
        return new DrinkItem("Хроно-суприм",
            "Квантовые зерна, звездный сахар, космическое молоко. Репутация++.",
            200);
    }
}

class EquipmentItem : public Item {
    int bonusEfficiency;
    int bonusReputation;
    Slot_Equipment slot;
public:
    EquipmentItem(const string& n,
        const string& d,
        int v,
        Slot_Equipment s,
        int be,
        int br)
        : Item(n, d, v, false),
        bonusEfficiency(be),
        bonusReputation(br),
        slot(s) {
    }

    int getBonusEfficiency() const { return bonusEfficiency; }
    int getBonusReputation() const { return bonusReputation; }
    Slot_Equipment getSlot() const { return slot; }

    void use(Player* user) override;
};

class Player : public Entity {
    int experience = 0;
    int expToNextLevel = 100;
    Inventory* inventory = nullptr;
    EquipmentItem* coffeeMachine = nullptr;
    EquipmentItem* interior = nullptr;
    int money = 0;
public:
    Player(const string& n, int exp, int m) : experience(exp), money(m) {
        name = n;
        inventory = new Inventory();
    }

    ~Player() override {
        delete inventory;
        delete coffeeMachine;
        delete interior;
    }

    void displayInfo() const override {
        cout << "Игрок: " << name << endl;
        cout << "Уровень: " << level << endl;
        cout << "Репутация: " << reputation << endl;
        cout << "Опыт: " << experience << "/" << expToNextLevel << endl;
        cout << "Деньги: " << money << endl;
        cout << "Энергия: " << energy << endl;
    }

    int getMoney() const { return money; }
    Inventory* getInventory() { return inventory; }

    void gainExp(int exp) {
        experience += exp;
        cout << "Получено " << exp << " опыта. Всего: " << experience << endl;
        if (experience >= expToNextLevel) {
            levelUp();
        }
    }

    void addMoney(int amount) { money += amount; }

    bool spendMoney(int amount) {
        if (money < amount) return false;
        money -= amount;
        return true;
    }

    void levelUp() {
        level++;
        reputation += 5;
        expToNextLevel += 50;
        experience -= expToNextLevel; // Исправлена ошибка
        cout << name << " повысил уровень до " << level << "!" << endl;
    }

    void equipItem(EquipmentItem* item) {
        if (!item) return;
        delete coffeeMachine;
        delete interior;
        coffeeMachine = item;
        interior = item;
        cout << "Установлено оборудование: " << item->getName() << endl;
    }
};

void EquipmentItem::use(Player* user) {
    if (!user) return;
    user->equipItem(this);
    cout << "Вы установили " << name
        << ". Эффективность +" << bonusEfficiency
        << ", репутация +" << bonusReputation << endl;
}

class Customer : public Entity {
protected:
    int patience = 100;
    int satisfaction = 50;
    int money = 50;
public:
    virtual ~Customer() = default;
    virtual void placeOrder() {
        cout << name << " формирует заказ...\n";
    }
    virtual void receiveDrink(Item* drink) {
        if (!drink) {
            cout << name << " не получил напиток и уходит недовольным.\n";
            satisfaction -= 20;
            return;
        }
        cout << name << " получает \"" << drink->getName()
            << "\" и платит " << money << " монет.\n";
        satisfaction += 20;
    }
    virtual void specialAbility(Player* player) = 0;

    void displayInfo() const override {
        cout << "Клиент: " << name << endl;
        cout << "Терпение: " << patience << endl;
        cout << "Удовлетворенность: " << satisfaction << endl;
        cout << "Деньги: " << money << endl;
    }
    int getMoney() const { return money; }
};

class KnightCustomer : public Customer {
public:
    KnightCustomer() {
        name = "Средневековый рыцарь";
        patience = 80;
        money = 80;
        satisfaction = 50;
    }
    void specialAbility(Player* player) override {
        cout << "Рыцарь: «Этот кофе крепче, чем удар меча!» Репутация +5." << endl;
        if (player)
            player->updateStatus(player->getLevel(), player->getReputation() + 5);
    }
};

class ScientistCustomer : public Customer {
public:
    ScientistCustomer() {
        name = "Викторианский ученый";
        patience = 90;
        money = 90;
        satisfaction = 50;
    }
    void specialAbility(Player* player) override {
        cout << "Учёный: «Поразительно… напиток, существующий вне времени!» Открываются новые идеи." << endl;
        if (player) player->gainExp(15);
    }
};

class SamuraiCustomer : public Customer {
public:
    SamuraiCustomer() {
        name = "Самурай-путешественник";
        patience = 120;
        money = 70;
        satisfaction = 60;
    }
    void specialAbility(Player* player) override {
        cout << "Самурай своим присутствием повышает репутацию кофейни. Репутация +8." << endl;
        if (player)
            player->updateStatus(player->getLevel(), player->getReputation() + 8);
    }
};

class SpaceExplorerCustomer : public Customer {
public:
    SpaceExplorerCustomer() {
        name = "Космический исследователь";
        patience = 90;
        money = 120;
        satisfaction = 55;
    }
    void specialAbility(Player* player) override {
        cout << "Исследователь приносит редкие ингредиенты с других планет. Немного опыта." << endl;
        if (player) player->gainExp(10);
    }
};

class TimeTouristCustomer : public Customer {
public:
    TimeTouristCustomer() {
        name = "Турист во времени";
        patience = 70;
        money = 50;
        satisfaction = 50;
    }
    void specialAbility(Player* player) override {
        cout << "Турист приводит новых посетителей. Репутация +3." << endl;
        if (player)
            player->updateStatus(player->getLevel(), player->getReputation() + 3);
    }
};

class AlienCustomer : public Customer {
public:
    AlienCustomer() {
        name = "Инопланетный путешественник";
        patience = 100;
        money = 130;
        satisfaction = 60;
    }
    void specialAbility(Player* player) override {
        cout << "Инопланетянин: «Ваш напиток соответствует стандартам моей цивилизации.» Репутация +10." << endl;
        if (player)
            player->updateStatus(player->getLevel(), player->getReputation() + 10);
    }
};

class HackerCustomer : public Customer {
public:
    HackerCustomer() {
        name = "Киберпанк-хакер";
        patience = 50;
        money = 150;
        satisfaction = 40;
    }
    void specialAbility(Player* player) override {
        cout << "Хакер временно ускоряет ваши кофейные машины. Немного опыта." << endl;
        if (player) player->gainExp(20);
    }
};

class ArtistCustomer : public Customer {
public:
    ArtistCustomer() {
        name = "Художник эпохи Возрождения";
        patience = 100;
        money = 70;
        satisfaction = 70;
    }
    void specialAbility(Player* player) override {
        cout << "Художник оставляет вдохновляющий отзыв. Репутация +7." << endl;
        if (player)
            player->updateStatus(player->getLevel(), player->getReputation() + 7);
    }
};

class TimeTraderCustomer : public Customer {
public:
    TimeTraderCustomer() {
        name = "Торговец временем";
        patience = 90;
        money = 200;
        satisfaction = 60;
    }
    void specialAbility(Player* player) override {
        cout << "Торговец предлагает странные товары. Немного денег и опыта." << endl;
        if (player) {
            player->addMoney(50);
            player->gainExp(10);
        }
    }
};

class VIPCustomer : public Customer {
public:
    VIPCustomer() {
        name = "Легендарный путешественник";
        patience = 120;
        money = 300;
        satisfaction = 70;
    }
    void specialAbility(Player* player) override {
        cout << "VIP платит невероятно щедро и открывает новую эпоху. Репутация +20, опыт +50." << endl;
        if (player) {
            player->updateStatus(player->getLevel(), player->getReputation() + 20);
            player->gainExp(50);
        }
    }
};

class Employee : public Entity {
protected:
    int salary = 0;
    int skillLevel = 1;
    int productivity = 1;
public:
    virtual ~Employee() = default;

    virtual void performTask() {
        cout << name << " выполняет задачу." << endl;
    }
    virtual void specialAbility(Player* player) = 0;

    void displayInfo() const override {
        cout << "Имя: " << name << endl;
        cout << "Зарплата: " << salary << endl;
        cout << "Навык: " << skillLevel << endl;
        cout << "Продуктивность: " << productivity << endl;
    }
};

class Barista : public Employee {
public:
    Barista(const string& n, int sal, int skill) {
        name = n;
        salary = sal;
        skillLevel = skill;
        productivity = 1 + skill;
    }

    void specialAbility(Player* player) override {
        cout << name << " ускоряет приготовление напитков. Клиенты довольнее." << endl;
        if (player) player->gainExp(5);
    }
};

class Recipe {
    Item* resultItem;
    map<string, int> requiredMaterials;
    int requiredLevel;
public:
    Recipe(Item* res, int reqLvl)
        : resultItem(res), requiredLevel(reqLvl) {
    }

    void addMaterial(const string& n, int c) {
        requiredMaterials[n] = c;
    }

    bool canCraft(Inventory* inv) {
        if (!inv) return false;
        // Упрощенная проверка - в реальной игре нужна полная проверка материалов
        return inv->getSize() >= 1;
    }

    void consumeMaterials(Inventory* inv) {
        cout << "Ингредиенты израсходованы." << endl;
    }

    Item* getResult() const { return resultItem; }
    int getRequiredLevel() const { return requiredLevel; }
};

class CraftStation {
    string stationType;
    vector<Recipe*> availableRecipes;
public:
    CraftStation(const string& t) : stationType(t) {}
    ~CraftStation() {
        for (auto* r : availableRecipes) delete r;
    }

    void addRecipe(Recipe* r) { availableRecipes.push_back(r); }

    void showRecipes() {
        cout << "=== Рецепты (" << stationType << ") ===\n";
        for (int i = 0; i < (int)availableRecipes.size(); ++i) {
            cout << i << ") " << availableRecipes[i]->getResult()->getName()
                << " (требуемый уровень: "
                << availableRecipes[i]->getRequiredLevel() << ")\n";
        }
    }

    bool craftItem(int idx, Inventory* inv, Player* player) {
        if (idx < 0 || idx >= (int)availableRecipes.size()) return false;
        Recipe* r = availableRecipes[idx];
        if (player->getLevel() < r->getRequiredLevel()) {
            cout << "Ваш уровень слишком низкий для этого рецепта.\n";
            return false;
        }
        if (!r->canCraft(inv)) {
            cout << "Недостаточно ингредиентов.\n";
            return false;
        }
        r->consumeMaterials(inv);
        Item* res = r->getResult();
        Item* crafted = new DrinkItem(res->getName(), res->getDescription(), res->getValue());
        inv->addItem(crafted);
        cout << "Создан напиток: " << crafted->getName() << "\n";
        return true;
    }
};

class Quest {
    string title;
    string description;
    QuestGoal objective;
    int requiredAmount;
    int currentAmount;
    int rewardGold;
    int rewardExp;
    bool isCompleted;
public:
    Quest(const string& t,
        const string& d,
        QuestGoal obj,
        int reqAmount,
        int gold,
        int exp)
        : title(t),
        description(d),
        objective(obj),
        requiredAmount(reqAmount),
        currentAmount(0),
        rewardGold(gold),
        rewardExp(exp),
        isCompleted(false) {
    }

    void updateProgress(int amount) {
        if (isCompleted) return;
        currentAmount += amount;
        cout << "Прогресс \"" << title << "\": "
            << currentAmount << "/" << requiredAmount << endl;
    }

    bool checkCompletion() {
        if (!isCompleted && currentAmount >= requiredAmount) {
            isCompleted = true;
        }
        return isCompleted;
    }

    void complete(Player* player) {
        if (!checkCompletion()) {
            cout << "Задание ещё не выполнено." << endl;
            return;
        }
        cout << "Задание \"" << title << "\" выполнено! Награда: "
            << rewardGold << " монет, " << rewardExp << " опыта." << endl;
        player->addMoney(rewardGold);
        player->gainExp(rewardExp);
    }

    const string& getTitle() const { return title; }
    const string& getDescription() const { return description; }
};

class TimeGuild {
    string guildName;
    int reputationLevel;
    vector<Quest*> availableQuests;
    vector<Quest*> activeQuests;
public:
    TimeGuild(const string& n)
        : guildName(n), reputationLevel(0) {
    }

    ~TimeGuild() {
        for (auto* q : availableQuests) delete q;
        for (auto* q : activeQuests) delete q;
    }

    void generateInitialQuests() {
        availableQuests.push_back(
            new Quest("Обслужить первых клиентов",
                "Обслужите 3 клиентов в ChronoCafe.",
                QuestGoal::ServeCustomers,
                3, 200, 50)
        );
        availableQuests.push_back(
            new Quest("Продать напитки",
                "Продайте 5 напитков.",
                QuestGoal::SellDrinks,
                5, 300, 80)
        );
    }

    void generateQuests(Player*) {
        if (availableQuests.empty()) generateInitialQuests();
    }

    void takeQuest(int index, Player*) {
        if (index < 0 || index >= (int)availableQuests.size()) return;
        activeQuests.push_back(availableQuests[index]);
        cout << "Вы взяли задание: " << availableQuests[index]->getTitle() << endl;
        availableQuests.erase(availableQuests.begin() + index);
    }

    void checkQuestProgress(Player* player) {
        for (auto* q : activeQuests) {
            if (q->checkCompletion()) {
                q->complete(player);
            }
        }
    }

    void showQuests() {
        cout << "=== Гильдия путешественников во времени ===\n";
        cout << "Доступные задания:\n";
        for (int i = 0; i < (int)availableQuests.size(); ++i) {
            cout << i << ") " << availableQuests[i]->getTitle()
                << " - " << availableQuests[i]->getDescription() << endl;
        }
    }

    vector<Quest*>& getActiveQuests() { return activeQuests; }
};

class Location {
    string name;
    int popularity;
    int difficulty;
public:
    Location(const string& n = "ChronoCafe", int pop = 1, int diff = 1)
        : name(n), popularity(pop), difficulty(diff) {
    }

    void generateCustomers() {
        cout << "Клиенты появляются в локации: " << name << endl;
    }

    void displayInfo() {
        cout << "=== Локация ===\n";
        cout << "Название: " << name << endl;
        cout << "Популярность: " << popularity << endl;
        cout << "Сложность: " << difficulty << endl;
    }

    const string& getName() const { return name; }
};

class GameWorld {
    vector<Location*> locations;
    Player* player;
    TimeGuild* guild;
    CraftStation* station;
    vector<Employee*> employees;
    int currentLocationId;

    Customer* generateRandomCustomerForLocation() {
        int loc = currentLocationId;
        int r = rand() % 10;
        if (loc == 1) {
            if (r < 6) return new KnightCustomer();
            return new TimeTouristCustomer();
        }
        else if (loc == 2) {
            if (r < 5) return new ScientistCustomer();
            return new ArtistCustomer();
        }
        else if (loc == 3) {
            if (r < 6) return new SamuraiCustomer();
            return new TimeTouristCustomer();
        }
        else if (loc == 4) {
            if (r < 4) return new SpaceExplorerCustomer();
            if (r < 7) return new HackerCustomer();
            return new AlienCustomer();
        }
        else if (loc == 5) {
            if (r < 7) return new AlienCustomer();
            return new SpaceExplorerCustomer();
        }
        else if (loc == 6) {
            if (r < 5) return new TimeTouristCustomer();
            return new TimeTraderCustomer();
        }
        else if (loc == 7) {
            if (r < 6) return new ArtistCustomer();
            return new ScientistCustomer();
        }
        else {
            if (r == 0) return new VIPCustomer();
            if (r < 3) return new KnightCustomer();
            if (r < 5) return new ScientistCustomer();
            if (r < 7) return new TimeTouristCustomer();
            return new SpaceExplorerCustomer();
        }
    }

    void showEmployees() {
        cout << "=== Ваши сотрудники ===\n";
        if (employees.empty()) {
            cout << "Пока никого не нанято.\n";
            return;
        }
        for (int i = 0; i < (int)employees.size(); ++i) {
            cout << i << ") ";
            employees[i]->displayInfo();
            cout << "\n";
        }
    }

    void shopMenu() {
        cout << "=== Магазин оборудования ===\n";
        cout << "1) Кофемашина (эфф. +3, реп. +2) — 500 монет\n";
        cout << "2) Неоновая подсветка (эфф. +1, реп. +5) — 400 монет\n";
        cout << "3) Дизайнерские диваны (эфф. +2, реп. +6) — 800 монет\n";
        cout << "4) Назад\n";

        int choice;
        cin >> choice;
        int price = 0;
        EquipmentItem* item = nullptr;

        switch (choice) {
        case 1:
            price = 500;
            item = new EquipmentItem("Кофемашина",
                "Базовая машина ChronoCafe",
                price, Slot_Equipment::CoffeeMachine, 3, 2);
            break;
        case 2:
            price = 400;
            item = new EquipmentItem("Неоновая подсветка",
                "Атмосфера будущего",
                price, Slot_Equipment::NeonLighting, 1, 5);
            break;
        case 3:
            price = 800;
            item = new EquipmentItem("Дизайнерские диваны",
                "Роскошный интерьер",
                price, Slot_Equipment::DesignerSofas, 2, 6);
            break;
        default: return;
        }

        if (!player->spendMoney(price)) {
            cout << "Недостаточно денег!\n";
            delete item;
            return;
        }
        cout << "Куплено: " << item->getName() << endl;
        player->getInventory()->addItem(item);
    }

    void hireEmployeeMenu() {
        cout << "=== Найм сотрудников ===\n";
        cout << "1) Бариста-новичок (зарплата 100, навык 1) — 600 монет\n";
        cout << "2) Бариста из будущего (зарплата 200, навык 3) — 1200 монет\n";
        cout << "3) Самурай-бариста (зарплата 150, навык 2) — 900 монет\n";
        cout << "4) Назад\n";
        int ch;
        cin >> ch;
        if (!cin) return;
        int price = 0;
        Barista* b = nullptr;
        switch (ch) {
        case 1:
            price = 600;
            b = new Barista("Бариста-новичок", 100, 1);
            break;
        case 2:
            price = 1200;
            b = new Barista("Бариста из будущего", 200, 3);
            break;
        case 3:
            price = 900;
            b = new Barista("Самурай-бариста", 150, 2);
            break;
        case 4:
        default:
            return;
        }
        if (!player->spendMoney(price)) {
            cout << "Недостаточно денег для найма.\n";
            delete b;
            return;
        }
        cout << "Вы наняли: " << b->getName() << "\n";
        employees.push_back(b);
    }

public:
    GameWorld(Player* p)
        : player(p),
        guild(new TimeGuild("Гильдия Хроно-странников")),
        station(new CraftStation("Лаборатория напитков")),
        currentLocationId(0) {

        locations.push_back(new Location("ChronoCafe", 1, 1));
        locations.push_back(new Location("Средневековый рынок", 2, 2));
        locations.push_back(new Location("Викторианский Лондон", 3, 3));
        locations.push_back(new Location("Япония эпохи самураев", 3, 3));
        locations.push_back(new Location("Космическая колония будущего", 4, 4));
        locations.push_back(new Location("Неизвестная инопланетная станция", 5, 5));
        locations.push_back(new Location("Туристический узел времени", 2, 1));
        locations.push_back(new Location("Эпоха Возрождения", 3, 2));

        guild->generateQuests(player);

        Recipe* r1 = new Recipe(DrinksFactory::makeEspressoTime(), 1);
        r1->addMaterial("кофейные зерна", 1);
        station->addRecipe(r1);

        Recipe* r2 = new Recipe(DrinksFactory::makeChronoLatte(), 1);
        r2->addMaterial("кофейные зерна", 1);
        r2->addMaterial("молоко", 1);
        r2->addMaterial("сахар", 1);
        station->addRecipe(r2);

        Recipe* r3 = new Recipe(DrinksFactory::makeSamuraiMatcha(), 2);
        r3->addMaterial("порошок матча", 1);
        r3->addMaterial("вода", 1);
        r3->addMaterial("мед", 1);
        station->addRecipe(r3);

        Recipe* r4 = new Recipe(DrinksFactory::makeVictorianMocha(), 2);
        r4->addMaterial("кофейные зерна", 1);
        r4->addMaterial("шоколад", 1);
        r4->addMaterial("молоко", 1);
        station->addRecipe(r4);

        Recipe* r5 = new Recipe(DrinksFactory::makeGalacticCappuccino(), 3);
        r5->addMaterial("кофейные зерна", 1);
        r5->addMaterial("молочная пена", 1);
        r5->addMaterial("звездный сахар", 1);
        station->addRecipe(r5);

        Recipe* r6 = new Recipe(DrinksFactory::makeQuantumCoffee(), 4);
        r6->addMaterial("квантовые зерна", 1);
        r6->addMaterial("кристальная вода", 1);
        station->addRecipe(r6);

        Recipe* r7 = new Recipe(DrinksFactory::makeAlienDrink(), 4);
        r7->addMaterial("инопланетные зерна", 1);
        r7->addMaterial("космическое молоко", 1);
        station->addRecipe(r7);

        Recipe* r8 = new Recipe(DrinksFactory::makeChronoSupreme(), 5);
        r8->addMaterial("квантовые зерна", 1);
        r8->addMaterial("звездный сахар", 1);
        r8->addMaterial("космическое молоко", 1);
        station->addRecipe(r8);

        employees.push_back(new Barista("Алиса, хроно-бариста", 100, 1));
        employees.push_back(new Barista("Такэши из эпохи самураев", 120, 2));
    }

    ~GameWorld() {
        for (auto* l : locations) delete l;
        for (auto* e : employees) delete e;
        delete guild;
        delete station;
    }

    void exploreLocation() {
        locations[currentLocationId]->generateCustomers();
    }

    void moveToLocation(int id) {
        if (id < 0 || id >= (int)locations.size()) {
            cout << "Такой эпохи нет.\n";
            return;
        }
        currentLocationId = id;
        cout << "Вы перемещаетесь в эпоху: "
            << locations[currentLocationId]->getName() << "\n";
    }

    void processCustomer(Customer* customer) {
        customer->placeOrder();
        customer->displayInfo();

        DrinkItem* drink = DrinksFactory::makeChronoLatte();
        cout << "Вы готовите напиток \"" << drink->getName() << "\"...\n";

        customer->receiveDrink(drink);
        int pay = customer->getMoney();
        player->addMoney(pay);
        cout << "Вы получили " << pay << " монет.\n";
        player->gainExp(10);
        customer->specialAbility(player);

        delete drink;

        for (auto* q : guild->getActiveQuests()) {
            q->updateProgress(1);
        }
        guild->checkQuestProgress(player);
    }

    void runGameLoop() {
        int choice = 0;
        while (true) {
            if (player->getMoney() <= 0) {
                cout << "\n=== БАНКРОТСТВО! ===\n";
                cout << "Вы не смогли спасти ChronoCafe от разорения.\n";
                cout << "Игра окончена.\n\n";
                return;
            }
            cout << "\n=== Меню ChronoCafe ===\n"
                << "1 - Ваша информация\n"
                << "2 - Обслужить клиента\n"
                << "3 - Выйти в город\n"
                << "4 - Выйти из игры\n\n";
            cin >> choice;
            if (!cin) return;

            switch (choice) {
            case 1: {
                player->displayInfo();
                locations[currentLocationId]->displayInfo();
                int ch;
                cout << "\nВыберите действие:\n"
                    << "1 - Ваш инвентарь\n"
                    << "2 - Ваши сотрудники\n"
                    << "3 - Ваше оборудование (через инвентарь)\n"
                    << "4 - Найм новых сотрудников\n"
                    << "5 - Назад\n";
                cin >> ch;
                if (!cin) return;
                switch (ch) {
                case 1:
                    player->getInventory()->displayContents();
                    break;
                case 2:
                    showEmployees();
                    break;
                case 3: {
                    player->getInventory()->displayContents();
                    cout << "Введите индекс предмета для установки (-1 для отмены): ";
                    int idx;
                    cin >> idx;
                    if (idx >= 0) {
                        Item* it = player->getInventory()->getItem(idx);
                        if (auto* eq = dynamic_cast<EquipmentItem*>(it)) {
                            eq->use(player);
                            player->getInventory()->removeItem(idx);
                        }
                        else {
                            cout << "Это не оборудование.\n";
                        }
                    }
                    break;
                }
                case 4:
                    hireEmployeeMenu();
                    break;
                case 5:
                    break;
                default:
                    cout << "Неизвестная команда.\n";
                    break;
                }
                break;
            }
            case 2: {
                cout << "К вам заходит клиент...\n";
                Customer* c = generateRandomCustomerForLocation();
                processCustomer(c);
                delete c;
                break;
            }
            case 3: {
                int ch;
                cout << "\nГород:\n"
                    << "1 - Лаборатория напитков\n"
                    << "2 - Гильдия путешественников во времени\n"
                    << "3 - Выбор эпохи\n"
                    << "4 - Магазин оборудования\n"
                    << "5 - Назад\n";
                cin >> ch;
                if (!cin) return;
                switch (ch) {
                case 1: {
                    station->showRecipes();
                    cout << "Введите индекс рецепта для крафта (-1 для отмены): ";
                    int r;
                    cin >> r;
                    if (r >= 0) {
                        station->craftItem(r, player->getInventory(), player);
                    }
                    break;
                }
                case 2: {
                    guild->showQuests();
                    cout << "Введите индекс задания для принятия (-1 для отмены): ";
                    int q;
                    cin >> q;
                    if (q >= 0) {
                        guild->takeQuest(q, player);
                    }
                    break;
                }
                case 3: {
                    cout << "Доступные эпохи:\n";
                    cout << "0 - ChronoCafe (база)\n";
                    cout << "1 - Средневековый рынок\n";
                    cout << "2 - Викторианский Лондон\n";
                    cout << "3 - Япония эпохи самураев\n";
                    cout << "4 - Космическая колония будущего\n";
                    cout << "5 - Неизвестная инопланетная станция\n";
                    cout << "6 - Туристический узел времени\n";
                    cout << "7 - Эпоха Возрождения\n";
                    cout << "Введите номер эпохи: ";
                    int loc;
                    cin >> loc;
                    moveToLocation(loc);
                    exploreLocation();
                    break;
                }
                case 4:
                    shopMenu();
                    break;
                case 5:
                    break;
                default:
                    cout << "Неизвестная команда.\n";
                    break;
                }
                break;
            }
            case 4:
                cout << "Спасибо за игру!\n";
                return;
            default:
                cout << "Неизвестная команда.\n";
                break;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned)time(nullptr));

    cout << "Где-то за гранью эпох и миров существует место,\n"
        << "о котором шепчут лишь самые опытные странники во времени.\n"
        << "Там, где пересекаются пути рыцарей, ученых, самураев и астронавтов,\n"
        << "расположено уютное кафе, стоящее вне законов пространства и времени.\n"
        << "Его имя известно немногим…  ChronoCafe.\n"
        << "Говорят, если странствовать достаточно долго,\n"
        << "в один день ты непременно окажешься именно здесь —\n"
        << "в легендарной кофейне, где время растворяется в аромате свежего кофе.\n"
        << "И сегодня именно вы — новый хозяин этого удивительного места.\n\n";

    string name;
    cout << "Введите ваше имя: ";
    getline(cin, name);

    Player player(name, 0, 10000);

    cout << name << ", поздравляем вас с новым рабочим местом! Угощайте гостей со всех эпох.\n"
        << "Открывайте порталы в новые временные эпохи. Отправляйтесь на поиски редких ингредиентов.\n"
        << "И сделайте ChronoCafe самым знаменитым заведением во всей временной линии!\n\n";

    cout << "Получено капитала: 10 000\n\n";

    GameWorld world(&player);
    world.runGameLoop();

    return 0;
}