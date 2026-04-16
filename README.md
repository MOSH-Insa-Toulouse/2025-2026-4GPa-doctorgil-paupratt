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

![alt text](./Project%20images/LTSpice/image.png)
*Circuit d'amplification/atténuation*

![alt text](./Project%20images/LTSpice/image-1.png)
*Modélisation du capteur*

Ce montage se compose de trois filtres passe-bas distincts pour optimiser le rapport signal/bruit :

* **Premier étage ($R_5, C_1, R_1$)** : Filtre les bruits en courant sur le signal d'entrée induits par l'alimentation 5V (symbolisée par 'SINE' + $C_3$).
* **Deuxième étage ($C_4, R_3$)** : Réduit spécifiquement la composante de bruit à **50 Hz** induite par le réseau électrique ambiant.
* **Troisième étage ($R_6, C_2$)** : Placé en sortie de l'amplificateur, il atténue le bruit thermique et intrinsèque du circuit.

Grâce à ce conditionnement, nous déterminons la résistance du capteur graphite par la formule suivante :

$$R_{meas} = \frac{V_{cc}}{V_{ADC}} \cdot R_1 \cdot \left(1 + \frac{R_3}{R_{potentio}}\right) - R_1 - R_5$$

### Analyse des simulations et dimensionnement

La simulation nous a permis d'optimiser deux paramètres critiques : le filtrage du bruit secteur et la dynamique de mesure.

#### 1. Optimisation du filtrage (C4 et R3)
L'étude fréquentielle montre l'impact direct des composants sur la qualité du signal :
* **Fréquence de coupure** : Plus $C_4$ est petit, plus la fréquence de coupure $f_c = \frac{1}{2\pi R_3 C_4}$ est élevée, ce qui réduit l'efficacité du filtrage des parasites à **50 Hz**.
* **Gain du filtre** : La résistance $R_3$ permet d'ajuster le décalage vertical du signal pour l'adapter à la fenêtre de lecture.

![Graphique de simulation LTSpice](./Project%20images/LTSpice/Simulation.png)
*Simulation de l'influence de C4 et R3 sur la stabilisation du signal.*


#### 2. Stratégie de Calibration et Plage Dynamique
Pour éviter la saturation de l'ADC de l'Arduino (limité à 5V), nous avons conçu un système de réglage de gain flexible :

* **Problématique** : La résistance des capteurs graphite varie énormément selon le dépôt. Un courant $I_{sens}$ trop élevé peut saturer l'amplificateur à 5V, rendant toute variation de pression invisible.
* **Solution Manuelle** : L'utilisateur peut ajuster dynamiquement le gain via le **potentiomètre numérique** en utilisant l'**encodeur rotatif**. 
* **Objectif** : L'utilisateur tourne l'encodeur pour ramener le signal au repos vers **2,5V** (valeur brute de 512 sur l'ADC). Cela place le point de fonctionnement au milieu de l'échelle de mesure, offrant une marge de manœuvre maximale pour observer les variations sans écrêtage.
#### Modélisation mathématique du capteur

Pour extraire la valeur de la résistance du capteur $R_c$ à partir de la tension mesurée $V_{adc}$, nous avons modélisé le circuit en régime statique (Basse Fréquence). 

En considérant l'AOP comme idéal en régime linéaire ($V_+ = V_-$) et en appliquant le théorème de **Millman**, nous obtenons les relations suivantes :

#### 1. Expression de $V_+$ (Diviseur de tension)
$$V_+ = \frac{R_1}{R_c + R_1 + R_5} \cdot V_{cc}$$

#### 2. Expression de $V_-$ (Théorème de Millman)
Comme l'entrée est reliée à la masse via $R_2$ :
$$V_- = \frac{\frac{V_{out}}{R_3} + \frac{0}{R_2}}{\frac{1}{R_3} + \frac{1}{R_2}} = V_+$$

#### 3. Équation finale de la résistance $R_c$
En posant $V_{out} = V_{adc}$ et en isolant $R_c$, nous arrivons à la formule de transfert utilisée dans le code Arduino :

$$R_c = R_1 \cdot \frac{V_{cc}}{V_{adc}} \cdot \left(1 + \frac{R_3}{R_2}\right) - R_1 - R_5$$

> [!IMPORTANT]
> Cette équation permet au microcontrôleur de convertir en temps réel la tension lue sur la broche analogique en une valeur de résistance physique exploitable pour la datasheet.



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

En parallèle de la partie *KiCad*, nous avons développé le [Code Arduino](./Arduino/Code/ProjetMosh/ProjetMosh.ino) permettant la communication des composants avec l'**Arduino UNO**, ainsi qu'avec le smartphone via l'application Android.

### Fonctionnalités principales :
* **Acquisition Analogique** : Lecture haute impédance et conversion du signal en résistance.
* **Contrôle du Gain via Encodeur** : Utilisation de l'**encodeur rotatif** pour incrémenter ou décrémenter la valeur du potentiomètre numérique. Cette interface permet à l'utilisateur de calibrer manuellement le capteur en temps réel.
* **Gestion du Potentiomètre Digital** : Communication via le bus I2C pour mettre à jour la résistance de contre-réaction ($R_2/R_g$) dès qu'une rotation de l'encodeur est détectée.
* **Interface Bluetooth** : Transmission des données vers l'application Android pour un monitoring sans fil.

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