# YSBD_Project1

HP:

>>*Η υλοποίηση του σωρού έγινε με την χρήση ενός struct σε κάθε ένα block. Κάθε block λοιπόν περιέχει ένα struct και μόνο.

>>Κάθε struct (block) περιέχει: 
-έναν πίνακα που αποτελείται από 5 records (όσα μπορεί να χωρέσει ένα block συμφωνα με τους υπολογισμούς μας ).
-ένα int που περιγράφει το πόσα records υπάρχουν εκείνη της στιγμή στο block.
-ένα boolean value που είναι true αν έχει υπάρξει διαγραφή στο block. 

>>Τα δύο τελευταία μέλη του block είναι χρήσιμα στις υλοποιήσεις insert and delete . Οι υλοποιήσεις αυτές κάθε αυτές δεν είναι περίπλοκες. 
-Η λογική της insert είναι ότι ψάχνει τα blocks ένα ένα αν έχουν διαθέσιμο χώρο τοσο επειδή δεν έχει έχει εισαχθεί ο μέγιστος αριθμός από records είτε έχει γιατί έχει υπάρξει διαγραφή και το record θα γίνει insert σε εκείνο το σημείο. Αν όλα τα blocks είναι γεμάτα , τότε δημιουργείται στο αρχείο ένα νέο block στο οποίο γίνεται insert το επικείμενο record.
-H delete σε πρώτη φάση εκτελεί αναζήτηση στο file και αν υπάρχει το record που είναι προς διαγραφή , τότε κάνει το id του -1 και σε κάθε πεδίο του γράφει τη λέξη “DELETED” ή αν δεν υπάρχει εκτυπώνει μήνυμα λάθους. H αλλαγή του id σε -1 αποσκοπεί στο να αναγνωρίζετε η εγγραφή αυτή ως διεγραμμένη και να είναι δυνατή η μελλοντική εκχώρηση κάποιας άλλης στη θέση της σε εκείνο το block. 
>>Η συνάρτηση HP_GetAllEntries εκτελεί μια αναζήτηση προπελάυντας το αρχείο απο το πρώτο block μέχρι και το μπλόκ που περιέχει την πληροφορία που έχει στο όρισμα . Στην περίπτωση που δεν υπάρχει εκτυπώνετε ανάλογο μήνυμα.
>>Τέλος, πρέπει να σημειώσουμε ότι η create δημιουργεί 2 blocks όπου το πρώτο περιέχει το απαραίτητο info για το heap και το επόμενο είναι το πρώτο που περιέχει records.

