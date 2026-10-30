# 46 — La revue du cœur bloque son intégration

## Défauts établis statiquement

La revue indépendante du candidat runtime relève quatre défauts, transmis au propriétaire pour tests de régression avant correction :

1. Après un timeout/interruption, le drainage des sorties sans borne peut attendre un descendant conservant les pipes ouverts, malgré l'arrêt du processus directement possédé.
2. Le journal `commands.jsonl` peut être remplacé par un symlink vers un fichier extérieur après préparation ; son append n'est pas encore précédé du contrôle d'appartenance requis.
3. Un résultat brut modifié et une empreinte actualisée seulement dans le manifeste peuvent rester contradictoires avec le reçu d'exécution sans faire perdre le rapport favorable.
4. Un fichier de configuration pertinent ajouté après l'exécution peut être ignoré par le rapport, qui parcourt seulement les anciens chemins.

Le coordinateur a recoupé les chemins avec le code relu. Ces constats sont statiques ; les expériences discriminantes restent à exécuter. Les 124 succès précédents et le symlink non exécuté ne démontraient pas ces contre-exemples.

## Correction bornée et sécurité des essais

L'auteur doit remettre les tests ciblés et leur confinement avant tout GREEN. Le test de drainage peut commencer par un double refusant un appel sans timeout ; tout essai réel parent/descendant doit disposer d'un nettoyage borné de ses seuls processus possédés. Le symlink reste conditionnel aux permissions OS, sans contournement ; tout SKIP est un gap explicite.

Les fichiers extérieurs visés par les tests appartiennent à leur fixture, jamais aux données utilisateur. Les contrôles de rapport doivent rester en lecture seule. Les tests acquis ne sont pas affaiblis. Le répertoire Windows doit aussi être aligné explicitement sur l'arbre isolé du helper avant intégration des adaptateurs.

## Portée

Intégration du cœur suspendue uniquement. Les scénarios intégrés et leurs preuves restent acquis ; Windows et la préparation QA du raccordement historique continuent dans leurs scopes autorisés. Aucun arrêt global ni nouvelle autorisation utilisateur nécessaire.

Avis natif `result_mvucgip4_001t_kbz906`, profil correctness-reviewer fresh, candidat `2631e5cab88dbd4e0af713a6f8458356c6ba55b2`. Localisations : `runner.py:41,53,61`, `evidence.py:286,430,466`, `reporting.py:118–140`. Aucun test, moteur, build ou édition par le reviewer.

La liaison de l'exécutable réellement lancé au reçu de build reste également une obligation de raccordement à préciser, pas un défaut démontré d'un adaptateur absent.
