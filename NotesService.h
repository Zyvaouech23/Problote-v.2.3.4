#ifndef NOTES_SERVICE_H
#define NOTES_SERVICE_H

class NotesService {
public:
    NotesService();
    ~NotesService();

    void createNote(const std::string &note);
    void deleteNote(int noteId);
    std::string getNote(int noteId) const;
    void updateNote(int noteId, const std::string &updatedNote);
};

#endif // NOTES_SERVICE_H