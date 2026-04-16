# 4A GP - Projet "Du capteur au banc de test" - I4PMH21

## Sommaire

* [Objectif du projet](#objectif-du-projet)
* [Philosophie du projet : Sobriété et Efficacité](#philosophie-du-projet--sobriété-et-efficacité)
* [Livrables](#livrables)
* [LTSpice](#ltspice)
* [KiCad](#kicad)
* [Shield](#shield)
* [Code Arduino](#code-arduino)
* [Application Android](#application-android)
* [Datasheet](#datasheet)

---

## Objectif du projet

Dans le cadre d'un cours dispensé lors du 2<sup>nd</sup> semestre de 4<sup>ème</sup> année de Génie Physique à l'INSA de Toulouse, il nous a été proposé de réaliser un **capteur low-tech à base de graphite**, puis d'en faire une analyse critique argumentée pour en cerner les potentialités mais également pour évoquer des solutions d'amélioration. Ainsi, ce projet nous a permis de balayer l'ensemble du domaine ; du capteur jusqu'à la réalisation d'une **datasheet** et du **banc de test**.

---
## Philosophie du projet : Sobriété et Efficacité

Face à l'urgence climatique et à la demande croissante de systèmes à faible empreinte carbone, ce projet s'inscrit dans une démarche de **sobriété technologique**. Simplifier les composants pour les rendre plus efficaces énergétiquement reste un défi majeur de l'ingénierie moderne.

Si la réalisation de capteurs à haute sensibilité possède une forte valeur ajoutée, il est primordial de questionner nos besoins réels : avons-nous systématiquement besoin d'un capteur ultra-précis s'il consomme davantage ? Ou un capteur plus simple, adapté à l'usage final, est-il suffisant ?

Le choix d'un **capteur graphite "low-tech"** illustre ce compromis (trade-off) entre précision et consommation. Plutôt que de viser la performance absolue au prix d'une complexité accrue, nous avons privilégié une solution optimisée, cohérente avec les ressources disponibles et les exigences réelles du cahier des charges.

---

## Livrables

L'ensemble du projet comprend les éléments suivants :

* **Un shield PCB** branché à une board **Arduino UNO** sur lequel nous retrouverons différents composants tels que :
    * Un capteur graphite
    * Un circuit d'amplification transimpédance
    * Un module Bluetooth
    * Un encodeur rotatoire
    * Un potentiomètre digital (*qui vient remplacer la résistance R2 du circuit d'amplification*)
    * Un capteur de contrainte commercial
* **Une simulation LTSpice** du circuit transimpédance
* **Un fichier KiCad** du shield avec l'ensemble des composants cités en amont
* **Un code Arduino** qui gère le fonctionnement et les communications des modules avec la board
* **Un fichier APK Android** (conçue à l'aide du site *MIT APP Inventor*) qui permet, à partir d'un smartphone Android, de gérer l'interface avec le shield Arduino UNO par le biais d'une communication Bluetooth
* **La datasheet** du capteur graphite

> [!TIP]
> **Conseil de lecture** : Pour une compréhension optimale du projet, commencez par consulter la **Datasheet** qui résume les performances théoriques avant d'explorer les fichiers de conception KiCad.
---

## LTSpice

Afin de valider la faisabilité du projet et d'anticiper le comportement dynamique de notre système, nous avons simulé la chaîne de conditionnement sous LTSpice.Le défi majeur réside dans l'impédance extrêmement élevée du capteur (de l'ordre du $G\Omega$), qui génère des courants infimes, de l'ordre du nanoampère ($nA$). Pour rendre ce signal exploitable par un microcontrôleur, il est impératif de le filtrer contre les bruits parasites et de l'amplifier de manière significative. Le montage de transimpédance présenté ci-dessous remplit cette fonction critique :

![alt text](./Project%20images/LTSpice/image-1.png)
Circuit d'amplification/atténuation

![alt text](./Project%20images/LTSpice/image-1.png)
Modélisation du capteur

Ce montage se compose de trois filtres passe-bas distincts pour optimiser le rapport signal/bruit :

* **Premier étage ($R_5, C_1, R_1$)** : Filtre les bruits en courant sur le signal d'entrée induits par l'alimentation 5V (symbolisée par 'SINE' + $C_3$).
* **Deuxième étage ($C_4, R_3$)** : Réduit spécifiquement la composante de bruit à **50 Hz** induite par le réseau électrique ambiant.
* **Troisième étage ($R_6, C_2$)** : Placé en sortie de l'amplificateur, il atténue le bruit thermique et intrinsèque du circuit.

Grâce à ce conditionnement, nous déterminons la résistance du capteur graphite par la formule suivante :

$$R_{meas} = \frac{V_{cc}}{V_{ADC}} \cdot R_1 \cdot \left(1 + \frac{R_3}{R_{potentio}}\right) - R_1 - R_5$$

Afin de valider le comportement du système, deux simulations ont été effectuées :
1. **Analyse de l'amplification** : Vérification de la dynamique du signal de sortie.
2. **Analyse spectrale** : Confirmation de l'atténuation des fréquences non souhaitées (réjection du 50 Hz).


## KiCad

### KiCad : Conception du Shield

Afin de concevoir un **shield PCB** conforme au cahier des charges de l'UF **I4PMH21**, nous avons développé un prototype virtuel sous **KiCad**. Cette phase de conception a permis d'intégrer l'ensemble des contraintes électroniques, mécaniques et physiques liées aux composants utilisés.

La première étape a consisté à élaborer la **saisie de schéma** (*schematic*) regroupant l'intégralité des modules :

![alt text](./Project%20images/KiCad//image-2.png)


### Routage du PCB (Layout)

Une fois le schéma validé, nous avons procédé au **routage du PCB**. L'enjeu principal a été la gestion de l'intégrité du signal dans un environnement mixte (analogique/numérique) :

* **Plan de masse** : Mise en place d'un plan de masse continu pour minimiser l'inductance de boucle et assurer une référence stable.
* **Isolation** : Séparation physique des pistes à haute impédance (liées au capteur graphite) pour limiter les couplages capacitifs.
* **Contraintes mécaniques** : Alignement des connecteurs pour un embrochage parfait sur l'Arduino UNO.

Pour maintenir l'intégrité du plan de masse et respecter les contraintes d'espace, nous avons opté pour la mise en place d'un **strap externe (jumper)**. Cette solution a permis de finaliser les connexions tout en garantissant une isolation optimale entre les pistes critiques.

Le design final du shield est présenté ci-dessous :

![alt text](./Project%20images/KiCad/image-3.png)


### Rendu 3D du Shield

Nous avons généré un rendu 3D du PCB pour visualiser l'encombrement final et l'agencement des composants. Voici une vue de dessus du projet :

![alt text](./Project%20images/KiCad/image-4.png)

> [!NOTE]
> **Ajustement des modèles 3D** : Vous remarquerez peut-être des chevauchements visuels sur le rendu. Cela s'explique par l'usage de modèles CAO (fichiers `.step`) standards dont l'encombrement diffère légèrement des composants réels soudés sur le banc de test. L'empreinte au sol (*footprint*) sur le cuivre est cependant parfaitement conforme aux composants physiques.

## Shield
Une fois la conception validée, nous avons procédé à la fabrication physique du shield sur une plaque d'époxy cuivrée à l'aide de Mme. Catherine Crouzet. Le processus de prototypage rapide a suivi les étapes classiques de la photogravure :

* **Insolation** : Transfert du typon (généré sous KiCad) sur la plaque photosensible via une exposition aux UV.

* **Révélation** : Élimination de la résine non exposée à l'aide d'un révélateur chimique.

* **Gravure** : Passage dans une solution de perchlorure de fer pour dissoudre le cuivre non protégé et révéler les pistes.

Après le nettoyage de la carte, nous avons procédé à l'assemblage. Cette étape a nécessité une grande précision, notamment pour le perçage des vias et le soudage des composants traversants (THT) et CMS, en veillant scrupuleusement au respect des contraintes de température pour ne pas endommager le capteur graphite.

(PHOTO)

> [!WARNING]
> **Points de vigilance (Soudure)** : Lors de l'assemblage, une attention extrême doit être portée à la qualité des soudures. Des micro-courts-circuits ou des contacts intermittents ("soudures sèches") ont été identifiés lors de nos tests initiaux, perturbant la stabilité du signal. Un nettoyage rigoureux et une vérification systématique au multimètre sont indispensables.

> [!NOTE]
> **Évolution du projet (Servomoteur)** : Bien que prévu initialement dans la conception du shield, le servomoteur n'a finalement pas été implémenté. Ce choix s'explique par des contraintes de temps et par le fait que le banc de test s'est révélé parfaitement fonctionnel et suffisant sans le servo pour valider les mesures du capteur.

## Code Arduino
## Code Arduino

En parallèle de la partie *KiCad*, nous avons développé le [Code Arduino]((./Arduino/Code/ProjetMosh.ino)) permettant la communication des composants avec l'**Arduino UNO**, ainsi qu'avec le smartphone via l'application Android.

### Fonctionnalités principales :
* **Acquisition Analogique** : Lecture du signal amplifié et conversion en résistance via la formule de transfert.
* **Pilotage du Potentiomètre Digital** : Ajustement dynamique du gain d'amplification via le bus I2C.
* **Interface Bluetooth** : Envoi périodique des données vers l'application Android.
* **Gestion de l'Encodeur** : Interface utilisateur physique pour naviguer dans les menus ou ajuster les seuils.

> [!TIP]
> **Structure du code** : Le code est organisé de manière non-bloquante (utilisation de `millis()` au lieu de `delay()`) pour garantir une réactivité maximale de la liaison Bluetooth et de l'encodeur.

---


## Application Android

> [!TIP]
> **Installation rapide** : Pour tester l'application Android sans recompiler le code, vous pouvez installer directement le fichier `.apk` situé dans le dossier `/Android/Release`.

*Interface utilisateur pour la visualisation des données en temps réel.*

## Datasheet
*Spécifications techniques du capteur de graphite réalisé.*

![alt text](./Project%20images/image-5.png)