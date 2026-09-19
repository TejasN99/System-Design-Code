#include<bits/stdc++.h>
using namespace std;

// ========================== DOMAIN ENTITIES ==========================

class Song {
private:
    string title;
    string artist;
    string path;
public:
    Song(const string &title, const string &artist, const string &path) : 
        title(title), artist(artist), path(path) {}

    string getSongTitle() const { return title; }
    string getSongArtist() const { return artist; }
    string getSongPath() const { return path; }
};

class Playlist {
private:
    string playlistName;
    vector<Song*> songs;
public:
    Playlist(const string& playName) : playlistName(playName) {}

    void addSongToPlaylist(Song *song) {
        if (song && find(songs.begin(), songs.end(), song) == songs.end()) {
            songs.push_back(song);
        }
    }

    const vector<Song*>& getPlaylistSongs() const { return songs; }
    string getPlaylistName() const { return playlistName; }
    int getSize() const { return songs.size(); }
};

class SongCatalog {
private:
    unordered_map<string, Song*> songs;
    SongCatalog() {}
public:
    static SongCatalog& getInstance() {
        static SongCatalog instance;
        return instance;
    }

    void addSong(const string& title, const string& artist, const string& path) {
        if (!songs.count(title)) {
            songs[title] = new Song(title, artist, path);
        }
    }

    Song* getSong(const string& title) {
        return songs.count(title) ? songs[title] : nullptr;
    }

    ~SongCatalog() {
        for (auto& p : songs) delete p.second;
        songs.clear();
    }
};

// ========================== AUDIO HARDWARE & ADAPTERS ==========================

class IAudioOutputDevice {
public:
    virtual void playAudio(Song *song) = 0;
    virtual ~IAudioOutputDevice() = default;
};

class Bluetooth {
public:
    void play(Song *song) {
        cout << " Song is played via bluetooth : " << song->getSongTitle() << endl;
    }
};

class BluetoothAdapter : public IAudioOutputDevice {
private:
    Bluetooth bt;
public:
    void playAudio(Song *song) override {
        bt.play(song);
    }
};

class WiredSpeaker {
public:
    void play(Song *song) {
        cout << " Song is played via WiredSpeaker : " << song->getSongTitle() << endl;
    }
};

class WiredSpeakerAdapter : public IAudioOutputDevice {
private:
    WiredSpeaker ws;
public:
    void playAudio(Song *song) override {
        ws.play(song);
    }
};

class Headphone {
public:
    void play(Song *song) {
        cout << " Song is played via Headphone : " << song->getSongTitle() << endl;
    }
};

class HeadphoneAdapter : public IAudioOutputDevice {
private:
    Headphone hp;
public:
    void playAudio(Song *song) override {
        hp.play(song);
    }
};

enum class DeviceType { 
    BLUETOOTH, 
    WIRED, 
    HEADPHONES 
};

class DeviceFactory {
public:
    static IAudioOutputDevice* createDevice(DeviceType dt) {
        switch(dt) {
            case DeviceType::BLUETOOTH:
                cout << "Bluetooth device connected" << endl;
                return new BluetoothAdapter();
            case DeviceType::WIRED:
                cout << "Wired device connected" << endl;
                return new WiredSpeakerAdapter();
            case DeviceType::HEADPHONES:
                cout << "Headphones connected" << endl;
                return new HeadphoneAdapter();
        }
        return nullptr;
    }
};

class AudioEngine {
private:
    Song* currSong;
public:
    AudioEngine() : currSong(nullptr) {}

    void playSong(IAudioOutputDevice* device, Song* song) {
        if (!device || !song) return;
        currSong = song;
        device->playAudio(song);
    }

    Song* getCurrentSong() const { return currSong; }
};

// ========================== PLAY STRATEGIES ==========================

class PlayStrategy {
public:
    virtual ~PlayStrategy() = default;
    virtual void setPlaylist(Playlist* pl) = 0;
    virtual bool hasNext() = 0;
    virtual Song* next() = 0;
    virtual bool hasPrev() = 0;
    virtual Song* prev() = 0;
    virtual void addToNext(Song* song) {}
};

class SequentialStrategy : public PlayStrategy {
private:
    Playlist* currList;
    int currIndex;
public:
    SequentialStrategy() : currList(nullptr), currIndex(-1) {}

    void setPlaylist(Playlist* pl) override {
        currList = pl;
        currIndex = -1;
    }

    bool hasNext() override {
        return currList && ((currIndex + 1) < currList->getSize());
    }

    Song* next() override {
        if (!hasNext()) {
            throw runtime_error("No Playlist loaded or reached end of playlist.");
        }
        currIndex++;
        return currList->getPlaylistSongs()[currIndex];
    }

    bool hasPrev() override {
        return currList && ((currIndex - 1) >= 0);
    }

    Song* prev() override {
        if (!hasPrev()) {
            throw runtime_error("No previous song available.");
        }
        currIndex--;
        return currList->getPlaylistSongs()[currIndex];
    }
};

class RandomStrategy : public PlayStrategy {
private:
    Playlist *currList;
    vector<Song*> remainSongs;
    vector<Song*> playHistory;
    int historyIndex;
public:
    RandomStrategy() : currList(nullptr), historyIndex(-1) {
        srand((unsigned)time(nullptr));
    }

    void setPlaylist(Playlist* pl) override {
        currList = pl;
        remainSongs.clear();
        playHistory.clear();
        historyIndex = -1;

        if (currList && currList->getSize() > 0) {
            remainSongs = currList->getPlaylistSongs();
        }
    }

    bool hasNext() override {
        return currList && (!remainSongs.empty() || historyIndex + 1 < (int)playHistory.size());
    }

    Song* next() override {
        if (!currList || currList->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }

        // Navigate forward through previously played history
        if (historyIndex + 1 < (int)playHistory.size()) {
            historyIndex++;
            return playHistory[historyIndex];
        }

        if (remainSongs.empty()) {
            throw runtime_error("No songs left to play");
        }

        int nextIndex = rand() % remainSongs.size();
        Song* nextSong = remainSongs[nextIndex];

        swap(remainSongs[nextIndex], remainSongs.back());
        remainSongs.pop_back();

        playHistory.push_back(nextSong);
        historyIndex++;
        return nextSong;
    }

    bool hasPrev() override {
        return historyIndex > 0;
    }

    Song* prev() override {
        if (!hasPrev()) {
            throw runtime_error("No previous song available.");
        }
        historyIndex--;
        return playHistory[historyIndex];
    }
};

class CustomQueueStrategy : public PlayStrategy {
private:
    Playlist* currList;
    int currIndex;
    queue<Song*> nextQueue;
    stack<Song*> history;

    Song* nextSequential() {
        if (!currList || currIndex + 1 >= currList->getSize()) {
            throw runtime_error("No sequential tracks available.");
        }
        currIndex++;
        return currList->getPlaylistSongs()[currIndex];
    }

    Song* previousSequential() {
        if (!currList || currIndex - 1 < 0) {
            throw runtime_error("No previous sequential tracks available.");
        }
        currIndex--;
        return currList->getPlaylistSongs()[currIndex];
    }

public:
    CustomQueueStrategy() : currList(nullptr), currIndex(-1) {}

    void setPlaylist(Playlist* pl) override {
        currList = pl;
        currIndex = -1;
        while(!nextQueue.empty()) nextQueue.pop();
        while(!history.empty()) history.pop();
    }

    bool hasNext() override {
        return !nextQueue.empty() || (currList && (currIndex + 1) < currList->getSize());
    }

    Song* next() override {
        if (!currList || currList->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        } 
        
        if (!nextQueue.empty()) {
            Song* s = nextQueue.front();
            nextQueue.pop();
            history.push(s);

            const auto& list = currList->getPlaylistSongs();
            for (int i = 0; i < (int)list.size(); ++i) {
                if (list[i] == s) {
                    currIndex = i;
                    break;
                }
            }
            return s;
        } 
        
        return nextSequential();
    }

    bool hasPrev() override {
        if (!history.empty()) return true;
        return currList && ((currIndex - 1) >= 0);
    }
    
    Song* prev() override {
        if (!currList || currList->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }

        if (!history.empty()) {
            Song* s = history.top();
            history.pop();

            const auto& list = currList->getPlaylistSongs();
            for (int i = 0; i < (int)list.size(); ++i) {
                if (list[i] == s) {
                    currIndex = i;
                    break;
                }
            }
            return s;
        }

        return previousSequential();
    }

    void addToNext(Song* song) override {
        if (!song) {
            throw runtime_error("Cannot enqueue null song.");
        }
        nextQueue.push(song);
    }
};

enum class PlayStrategyType { 
    SEQUENTIAL, 
    RANDOM, 
    CUSTOM_QUEUE 
};

class StrategyFactory {
public:
    static PlayStrategy* createStrategy(PlayStrategyType type) {
        switch(type) {
            case PlayStrategyType::SEQUENTIAL:   return new SequentialStrategy();
            case PlayStrategyType::RANDOM:       return new RandomStrategy();
            case PlayStrategyType::CUSTOM_QUEUE: return new CustomQueueStrategy();
        }
        return nullptr;
    }
};

// ========================== SESSION CONTROLLER / FACADE ==========================

class MusicPlayerFacade {
private:
    AudioEngine* aud;
    IAudioOutputDevice* currentOutputDevice;
    Playlist* loadedPlaylist;
    PlayStrategy* playStrat;

public:
    MusicPlayerFacade() 
        : aud(new AudioEngine()), currentOutputDevice(nullptr), 
          loadedPlaylist(nullptr), playStrat(nullptr) {}

    ~MusicPlayerFacade() {
        delete aud;
        delete currentOutputDevice;
        delete playStrat;
    }

    void connectDevice(DeviceType dt) {
        delete currentOutputDevice;
        currentOutputDevice = DeviceFactory::createDevice(dt);
    }

    void setPlayStrategy(PlayStrategyType stratType) {
        delete playStrat;
        playStrat = StrategyFactory::createStrategy(stratType);
        if (loadedPlaylist && playStrat) {
            playStrat->setPlaylist(loadedPlaylist);
        }
    }

    void loadPlaylist(Playlist* pl) {
        loadedPlaylist = pl;
        if (playStrat) {
            playStrat->setPlaylist(loadedPlaylist);
        }
    }

    void playSong(Song* song) {
        if (!currentOutputDevice) {
            throw runtime_error("No audio device connected.");
        }
        aud->playSong(currentOutputDevice, song);
    }

    void playAllTracks() {
        if (!loadedPlaylist) throw runtime_error("No playlist loaded.");
        if (!playStrat) throw runtime_error("Play strategy not set.");
        if (!currentOutputDevice) throw runtime_error("No audio device connected.");

        while (playStrat->hasNext()) {
            Song* nextSong = playStrat->next();
            aud->playSong(currentOutputDevice, nextSong);
        }
        cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << endl;
    }

    void playNextTrack() {
        if (!loadedPlaylist) throw runtime_error("No playlist loaded.");
        if (!playStrat) throw runtime_error("Play strategy not set.");
        if (!currentOutputDevice) throw runtime_error("No audio device connected.");

        if (playStrat->hasNext()) {
            Song* nextSong = playStrat->next();
            aud->playSong(currentOutputDevice, nextSong);
        } else {
            cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << endl;
        }
    }

    void playPreviousTrack() {
        if (!loadedPlaylist) throw runtime_error("No playlist loaded.");
        if (!playStrat) throw runtime_error("Play strategy not set.");
        if (!currentOutputDevice) throw runtime_error("No audio device connected.");

        if (playStrat->hasPrev()) {
            Song* prevSong = playStrat->prev();
            aud->playSong(currentOutputDevice, prevSong);
        } else {
            cout << "Reached start of playlist: " << loadedPlaylist->getPlaylistName() << endl;
        }
    }

    void enqueueNext(Song* song) {
        if (!playStrat) throw runtime_error("Play strategy not set.");
        playStrat->addToNext(song);
    }
};

// ========================== USER MANAGEMENT ==========================

class User {
private:
    string userId;
    string userName;
    unordered_map<string, Playlist*> userPlaylists;
    MusicPlayerFacade* player;

public:
    User(const string& id, const string& name) 
        : userId(id), userName(name), player(new MusicPlayerFacade()) {}

    ~User() {
        for (auto& pair : userPlaylists) {
            delete pair.second;
        }
        delete player;
    }

    string getId() const { return userId; }

    void createPlaylist(const string& playlistName) {
        if (!userPlaylists.count(playlistName)) {
            userPlaylists[playlistName] = new Playlist(playlistName);
        }
    }

    Playlist* getPlaylist(const string& playlistName) {
        return userPlaylists.count(playlistName) ? userPlaylists[playlistName] : nullptr;
    }

    MusicPlayerFacade* getPlayer() {
        return player;
    }
};

class UserManager {
private:
    unordered_map<string, User*> users;
    UserManager() {}
public:
    static UserManager& getInstance() {
        static UserManager instance;
        return instance;
    }

    User* registerUser(const string& userId, const string& name) {
        if (!users.count(userId)) {
            users[userId] = new User(userId, name);
        }
        return users[userId];
    }    
    
    User* getUser(const string& userId) {
        return users.count(userId) ? users[userId] : nullptr;
    }

    ~UserManager() {
        for (auto& p : users) delete p.second;
        users.clear();
    }
};

// ========================== MAIN EXECUTION ==========================

int main() {
    try {
        SongCatalog& catalog = SongCatalog::getInstance();
        catalog.addSong("Kesariya", "Arijit Singh", "/path/k.mp3");
        catalog.addSong("Chaiyya Chaiyya", "Sukhwinder Singh", "/path/c.mp3");

        UserManager& userMgr = UserManager::getInstance();
        User* alice = userMgr.registerUser("u1", "Alice");
        User* bob   = userMgr.registerUser("u2", "Bob");

        alice->createPlaylist("Workout");
        alice->getPlaylist("Workout")->addSongToPlaylist(catalog.getSong("Kesariya"));

        bob->createPlaylist("Workout");
        bob->getPlaylist("Workout")->addSongToPlaylist(catalog.getSong("Chaiyya Chaiyya"));

        // Alice connects Headphones and sets Sequential playback
        alice->getPlayer()->connectDevice(DeviceType::HEADPHONES);
        alice->getPlayer()->setPlayStrategy(PlayStrategyType::SEQUENTIAL);
        alice->getPlayer()->loadPlaylist(alice->getPlaylist("Workout"));

        // Bob connects Bluetooth and sets Random playback without clobbering Alice
        bob->getPlayer()->connectDevice(DeviceType::BLUETOOTH);
        bob->getPlayer()->setPlayStrategy(PlayStrategyType::RANDOM);
        bob->getPlayer()->loadPlaylist(bob->getPlaylist("Workout"));

        cout << "\nAlice starts playback:\n";
        alice->getPlayer()->playNextTrack();

        cout << "\nBob starts playback:\n";
        bob->getPlayer()->playNextTrack();

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}