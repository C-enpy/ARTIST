# 63 — Revue indépendante nativeRead reçue et acceptée à sa portée

2026-10-26. Suite de62, candidat source961d92e et preuves37f2b58. Deux fragments publics de pool-10/correctness-reviewer et FIN_REVIEW_NATIVE récupérés intégralement par lecture native ciblée des appels team_message après team_results sans message non lu. Pas de nouvelle analyse demandée ; arguments publics seulement, aucun raisonnement privé. Admission fresh/profil/lecture/transport déjà vérifiés62.

## Remise indépendante

Deux corps lus intégralement : native_read.hpp1–63 et controls.cpp1–111, consommateur privé standalone Win32 synchrone unique, parent de fixture fiable sans mutation concurrente. SHA attribués au contrôle lead, pas recalculés par reviewer. Aucun test/shell/édition/board/délégation ou accès aux justifications auteurs.

**Aucun défaut démontré dans ce contrat borné.** Boucle jusqu'à succès/n0, lecture courte poursuivie, erreur capturée avant diagnostic et append réservé aux reads réussis ; préfixe/erreur primaire conservés. Requête8192 sûre en DWORD, total size_t, max_size-size avant append. Handle non copiable, tentative close unique par invalidation préalable, unwind d'allocation couvert statiquement. Évaluation agrégée bytes.size() avant move ; aucun emprunt pendant réallocation ou retour.

Fixture : acquisition exclusive, noms possédés seulement, pas adoption de racine existante ni suppression récursive/voisins. Tailles fixes représentables par WriteFile ; comptes/flush/close exigés. Comparaison taille et string complète incluant NUL/contenu après8192 ; quatre assertions non court-circuitées sur résultat faux. Exception invalide la campagne avec cleanup.

## Limites distinctes de findings

ReadFile authentiquement échoué après préfixe, allocation échouée et CloseHandle échoué ne sont pas exécutés. Scénarios futurs proposés : préfixe+okfalse+erreur originale ; fermeture unique sur unwind ; close échoué invalide lecture correcte, primaire de lecture préservée sinon. Aucune seam/expérience supplémentaire adoptée ici. ODR multi-TU, fixtures arbitraires >DWORD, concurrence et intégration moteur hors contrat actuel, non transformés en bugs présents.

## Décision lead et état

Revue acceptée comme contrôle indépendant source sans finding bloquant, pas PASS exhaustif/global. Preuves RED58/GREEN62 réutilisées sans rerun :4lectures et2contratsCLI distincts. M0/M1/M2 stdio, close/catch/Reader restent non prouvés. Tâche REVIEW_NATIVE_READ peut être complétée après commit de ce rapport ; consommateur stdio peut être préparé ensuite, pas déclaré implémenté. Les anciens failed/blocked de l'attenteGO ne sont pas findings comportementaux.

HEAD37f2b58 contrôlé à la reprise, aucun diffARTIST ; diff concurrent de harness/profils/GrepAI préservé. Aucun build/test répété, aucun push ou backend de présentation démarré.
